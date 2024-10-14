#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>

namespace utils::io{
    
    std::string input(std::string message);
    void input();
    
	class Print{
	public:
		template<class T>
		static void print(T el, char end = '\n', char start = '\r'){
			std::cout<< start <<el<<end;
		}

		template<class T>
		static void print(std::vector<T> vec, char end = '\n', char start = '\r'){
			std::cout << start <<'[';
			for(int i = 0; i < vec.size(); i++){
				std::cout<<'"';
				print(vec[i], ' ', ' ');
				if(i != vec.size()-1){
					std::cout<<"\", ";
				} else {
					std::cout<<'"';
				}
			}
			std::cout<<']'<<end;
		}

		template<class K, class V>
		static void print(std::pair<K, V> el, char end = '\n', char start = '\r'){
			std::cout << start << '{';
			print(el.first, ' ', ' '); std::cout << ':';
			print(el.second, ' ', ' '); std::cout << '}';
		}

		template<class K, class V>
		static void print(std::map<K, V> obj, char end = '\n', char start = '\r'){
			int i = 0;
			for(auto &pair: obj){
				std::cout << start << '{';
				print(pair.first, ' ', ' '); std::cout << ':';
				print(pair.second, ' ', ' '); std::cout << '}';
				if(i < obj.size() - 1)
					std::cout << ',' << std::endl;
				i++;
			}
			std::cout << end;
		}

		template<typename... Args>
		static void print(const std::tuple<Args...>& tuple, char end = '\n', char start = '\r') {
			std::cout << start << '(';
			print_tuple_helper(tuple, std::index_sequence_for<Args...>{});
			std::cout << ')' << end;
		}

	private:
		template<typename Tuple, std::size_t... Is>
		static void print_tuple_helper(const Tuple& tuple, std::index_sequence<Is...>) {
			((print_tuple_element(std::get<Is>(tuple), Is == sizeof...(Is) - 1)), ...);
		}

		template<typename T>
		static void print_tuple_element(const T& element, bool is_last) {
			print(element, ' ', ' ');
			if (!is_last) {
				std::cout << ", ";
			}
		}
	};

	std::string input(std::string message){
		std::cout<<message;
		std::getline(std::cin, message);
		return message;
	}

	void input(){
		std::cout<<"> ";
		char buf;
		std::cin>>buf;
	}

}