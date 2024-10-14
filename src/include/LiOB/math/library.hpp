#pragma once

#include "random.hpp"
#include <utils/string.hpp>
#include <utils/vector.hpp>

#include "config.hpp"
#include "address.hpp"
#include "math.hpp"

namespace LiOB{

    std::string fill_with_spaces(std::string phrase, int max_len){
        if(phrase.size() >= max_len)
            return phrase;

        int n = max_len - phrase.size();
        for(int i = 0; i < n; i++){
            phrase += " ";
        }

        return phrase;
    }

    class Library{
            LiOB_config config;

            lint __seed_constant() { return lint{config.charset.size()} ^ lint{max_simbols()}; }

            lint __get_seed(
                lint address,
                lint room_uid
            ){ return address - room_uid * __seed_constant(); }

            lint __get_addr(
                lint seed,
                lint room_uid
            ){ return seed + room_uid * __seed_constant(); }

            lint __aprox_content_search(
                lint query_len,
                lint simbols_per_text,
                lfloat log_texts_num,
                lint base = 29
            ){
                lfloat probability = lfloat(simbols_per_text - query_len + 1) / simbols_per_text.convert_to<lfloat>();
                if (probability <= 0) {
                    throw std::runtime_error("Probability must be greater than 0, now it is " + convert_str(probability));
                }
                lfloat log_prob = blog(probability, base.convert_to<lfloat>());
                if (!std::isfinite(log_prob.convert_to<double>()) || !std::isfinite(log_texts_num.convert_to<double>())) {
                    throw std::runtime_error("Logarithm result is not finite.");
                }
                lint n = lint{std::floor((log_prob + log_texts_num).convert_to<double>())};
                return n;
            }

            lint __aprox_page_content_search(
                lint query_len,
                lint base = 29
            ){
                lfloat log_num_of_texts = lfloat{max_simbols()} * lfloat{lint{config.pages_no}} * blog(config.charset.size(), base.convert_to<lfloat>()) + blog(config.pages_no, base.convert_to<lfloat>());
                lint num_of_simbs = max_simbols();

                return __aprox_content_search(query_len, num_of_simbs, log_num_of_texts, base);
            }

            lint __aprox_title_search(
                lint title_len,
                lint base = 29
            ){
                lfloat log_num_of_texts = lfloat{max_simbols()} * blog(config.charset.size(), base.convert_to<lfloat>());
                lint num_of_simbs = 25;

                return __aprox_content_search(title_len, num_of_simbs, log_num_of_texts, base);
            }

            

            std::string normalize_text(std::string text){
                std::transform(text.begin(), text.end(), text.begin(), tolower);
                text.erase(std::remove_if(text.begin(), text.end(), [this](char c){ return std::find(config.charset.begin(), config.charset.end(), c) == config.charset.end(); }), text.end());
                return text;
            }

        public:
        
            lint max_simbols() const noexcept { return config.lines_no * config.simbs_no; }
            
            
            std::string get_book_uid(
                const LiOB::LiOB_address &address
            ){
                std::string s_wall = std::to_string(address.wall);
                std::string s_shelf = std::to_string(address.shelf);
                std::string s_volume = std::to_string(address.volume);

                if(s_volume.size() < 2){
                    s_volume = "0" + s_volume;
                }

                return s_volume + s_shelf + s_wall;
            }

            std::string get_page_uid(
                const LiOB::LiOB_address &address
            ){
                std::string s_page = std::to_string(address.page);
                
                if(s_page.size() < 3){
                    std::string tmp;
                    for(int i = 0; i < 3 - s_page.size(); i++){
                        tmp += "0";
                    }

                    s_page = tmp + s_page;
                }

                return s_page + get_book_uid(address);
            }

            LiOB_address addressGen(
                const std::string &phrase,
                const std::string &page_uid
            ){
                std::string nphrase = normalize_text(phrase);

                const int n = max_simbols().convert_to<int>() - nphrase.size();
                const int i_page_uid = std::stoi(page_uid);

                // eml::setseed(ghash(seed));
                for(int i = 0; i < n; i++){
                    // result += eml::choice(utils::vec::brakestring(config.charset));
                    nphrase += " ";
                }

                lint seed = 0;
                for(int i = 0; i < nphrase.size(); i++){
                    int charval = 0;

                    if(isalpha(nphrase[i])){
                        charval = static_cast<int>(tolower(nphrase[i])) - static_cast<int>('a');
                    } else if(nphrase[i] == '.'){
                        charval = 28;
                    } else if(nphrase[i] == ','){
                        charval = 27;
                    }

                    seed += i_page_uid * __seed_constant() + charval;
                }

                LiOB_address addr;
                addr.room_uid = convert_base(convert_str(seed), 10, 36);
                addr.wall = std::stoi(utils::str::to_string(page_uid[6]));
                addr.shelf = std::stoi(utils::str::to_string(page_uid[5]));
                addr.volume = std::stoi(page_uid.substr(3, 2));
                addr.page = std::stoi(page_uid.substr(0, 3));
                std::cout << "page: " << addr.page << std::endl;

                return addr;
            }

            std::string contentGen(
                const LiOB::LiOB_address &address
            ){
                lint page_uid = lint(get_page_uid(address));
                lint seed = __get_seed(
                    lint(convert_base(address.room_uid, 36, 10)), 
                    page_uid
                );

                std::string result = convert_base(convert_str(seed), 10, 29, config.charset);

                if(result.size() > max_simbols()){
                    result = result.substr(0, max_simbols().convert_to<int>());
                }

                int n = max_simbols().convert_to<int>() - result.size();

                eml::setseed(ghash(seed));
                for(int i = 0; i < n; i++){
                    result += eml::choice(utils::vec::brakestring(config.charset));
                }

                return result;
            }

            lint aprxSearch(
                lint query_len,
                lint base = 29
            ){
                return __aprox_page_content_search(query_len, base);
            }

            lint aprxTitleSearch(
                lint title_len,
                lint base = 29
            ){
                return __aprox_title_search(title_len, base);
            }

            Library(
                const LiOB_config& config
            ): config(config) {}

            Library(){}
            ~Library() = default;
    };
}