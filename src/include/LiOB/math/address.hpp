#pragma once

#include <utils/string.hpp>
#include <MathLib/Random.hpp>
#include <fmt/core.h>

#include "math.hpp"

namespace LiOB{
    struct LiOB_address{
        std::string room_uid;

        int wall;
        int shelf;
        int volume;

        int page = -1;

        std::string to_string(){
            return fmt::format("{}-{}w-{}s-{}v:{}", room_uid, wall, shelf, volume, page);
        }
    };

    std::string LiOB_random_room_uid(int len = 36){
        eml::setseed(ghash(time(0)));
        std::string uid;

        for(int i = 0; i < len; i++)
            uid += eml::choice(utils::vec::brakestring("abcdefghijklmnopqrstuvwxyz0123456789"));

        return uid;
    }
}