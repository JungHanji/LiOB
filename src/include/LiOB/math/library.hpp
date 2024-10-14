#pragma once

#include <Logging/logger.hpp>
#include "random.hpp"

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
            logging::Logger logger;

            lint __seed_constant() { return lint{config.charset.size()} ^ lint{max_simbols()}; }

            lint __get_seed(
                lint address,
                lint room_uid
            ){
                logger.newlayer("__get_seed");
                logger.log(logging::INFO, "address: " + convert_str(address));
                logger.log(logging::INFO, "room_uid: " + convert_str(room_uid));
                logger.poplayer();
                return address - room_uid * __seed_constant(); 
            }

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
                logger.newlayer("get_page_uid");
                logger.log(logging::INFO, "book uid: " + get_book_uid(address));
                logger.log(logging::INFO, "page uid: " + s_page + get_book_uid(address));
                logger.poplayer();
                return s_page + get_book_uid(address);
            }

            LiOB_address addressGen(
                const std::string &phrase,
                const std::string &page_uid
            ){
                logger.newlayer("addressgen");
                std::string nphrase = normalize_text(phrase);

                const int n = max_simbols().convert_to<int>() - nphrase.size();
                const int i_page_uid = std::stoll(page_uid);

                logger.log(logging::INFO, "phrase len: " + std::to_string(nphrase.size()));

                lint seed = 0;
                for(int i = nphrase.size(); i >= 0; i--){
                    int charval = 0;

                    if(isalpha(nphrase[i])){
                        charval = (int)nphrase[i] - (int)'a';
                    } else if(nphrase[i] == '.'){
                        charval = 28;
                    } else if(nphrase[i] == ','){
                        charval = 27;
                    }

                    seed += lint{(int)charval} * lint{config.charset.size()}^i;
                }

                logger.log(logging::INFO, "seed: " + convert_str(seed).substr(0, 100));
                logger.log(logging::INFO, "input PAGE_UID string: " + page_uid);

                LiOB_address addr;
                addr.room_uid = convert_base(convert_str(seed), 10, 36);
                addr.wall = std::stoll(utils::str::to_string(page_uid[6]));
                addr.shelf = std::stoll(utils::str::to_string(page_uid[5]));
                addr.volume = std::stoll(page_uid.substr(3, 2));
                addr.page = std::stoll(page_uid.substr(0, 3));
                
                logger.log(logging::INFO, "room id: " + convert_str(addr.room_uid).substr(0, 100));
                logger.log(logging::INFO, "wall id: " + convert_str(addr.wall));
                logger.log(logging::INFO, "shelf id: " + convert_str(addr.shelf));
                logger.log(logging::INFO, "volume id: " + convert_str(addr.volume));
                logger.log(logging::INFO, "page id: " + convert_str(addr.page));

                logger.poplayer();
                return addr;
            }

            std::string contentGen(
                const LiOB::LiOB_address &address
            ){
                logger.newlayer("contentgen");

                logger.log(logging::INFO, "room_uid: " + address.room_uid);

                lint page_uid = lint{std::stoll(get_page_uid(address))};
                lint seed = __get_seed(
                    lint(convert_base(address.room_uid, 36, 10)), 
                    page_uid
                );

                logger.log(logging::INFO, "page_uid (lint -> str): " + convert_str(page_uid));
                logger.log(logging::INFO, "seed: " + convert_str(seed).substr(0, 100));
                logger.log(logging::INFO, "wall id: " + convert_str(address.wall));
                logger.log(logging::INFO, "shelf id: " + convert_str(address.shelf));
                logger.log(logging::INFO, "volume id: " + convert_str(address.volume));
                logger.log(logging::INFO, "page id: " + convert_str(address.page));

                std::string result = convert_base(convert_str(seed), 10, 29, config.charset);

                if(result.size() > max_simbols()){
                    logger.log(logging::WARNING, "Too big amount of numbers: " + convert_str(result.size()));
                    result = result.substr(0, max_simbols().convert_to<int>());
                    logger.log(logging::INFO, "content: " + result.substr(0, 100));
                    logger.poplayer();
                    return result;
                } else if (result.size() == max_simbols()){
                    logger.log(logging::INFO, "content: " + result.substr(0, 100));
                    logger.poplayer();
                    return result;
                }

                logger.log(logging::WARNING, "Not enougth simbols: " + convert_str(result.size()));
                logger.log(logging::INFO, "content for now: " + result.substr(0, 100));
                
                int n = max_simbols().convert_to<int>() - result.size();
                long long nseed = ghash(seed);
                eml::setseed(nseed);
                logger.log(logging::INFO, "Setted new seed: " + convert_str(nseed));
                for(int i = 0; i < n; i++){
                    result += eml::choice(utils::vec::brakestring(config.charset));
                }

                logger.log(logging::INFO, "content: " + result.substr(0, 100));
                logger.poplayer();
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
            ): config(config), logger(logging::BOTH ,"log_exe") {}

            Library(){}
            ~Library() = default;
    };
}