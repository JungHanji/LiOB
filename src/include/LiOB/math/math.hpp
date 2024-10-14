#pragma once

#include "config.hpp"

#include <stdexcept>
#include <algorithm>
#include <functional>
#include <cmath>

namespace LiOB{

    std::string convert_base(const std::string& number, lint from_base, lint to_base, const std::string& custom_digits_out = "", const std::string& custom_digits_in = "") {
        if (from_base < 2 || from_base > 36 || to_base < 2 || to_base > 36) {
            throw std::invalid_argument("Bases must be in the range [2, 36]");
        }

        // Проверка на отрицательное число
        bool is_negative = number[0] == '-';
        std::string abs_number = is_negative ? number.substr(1) : number; // Убираем знак для обработки

        // Используем стандартные символы, если набор пустой
        const std::string digits_in = custom_digits_in.empty() ? "0123456789abcdefghijklmnopqrstuvwxyz" : custom_digits_in;
        const std::string digits_out = custom_digits_out.empty() ? "0123456789abcdefghijklmnopqrstuvwxyz" : custom_digits_out;

        // Convert from 'from_base' to decimal
        lint decimal_value = 0;
        for (char digit : abs_number) { // Используем абсолютное значение
            lint index = digits_in.find(tolower(digit));
            if (index == std::string::npos || index >= from_base) {
                throw std::invalid_argument("Invalid character in number: " + utils::str::to_string(digit));
            }
            decimal_value = decimal_value * from_base + index;
        }

        // Convert from decimal to 'to_base'
        std::string result;
        do {
            lint remainder = decimal_value % to_base;
            result += digits_out[remainder.convert_to<int>()]; // Используем набор символов
            decimal_value /= to_base;
        } while (decimal_value > 0);

        std::reverse(result.begin(), result.end());

        // Добавляем знак обратно, если число было отрицательным
        if (is_negative) {
            result = "-" + result;
        }

        return result;
    }

    lfloat blog(const lfloat &num, const lfloat &base){
        return bmul::log(num) / bmul::log(base);
    }

    template<class T>
    std::string convert_str(T v){
        std::ostringstream oss;
        oss << v;
        return oss.str();
    }

    template<class T>
    long long ghash(T val){
        std::hash<T> hashfun;
        return hashfun(val);
    }
}