#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <utils/string.hpp>

#include <utils/string.hpp>
#include <utils/vector.hpp>

namespace LiOB{

    namespace bmul = boost::multiprecision;
    using lint   = bmul::cpp_int;
    using lfloat = bmul::cpp_dec_float_100;

    struct LiOB_config{
        std::string charset = "abcdefghijklmnopqrstuvwxyz, .";

        int walls_no = 4;
        int shelves_no = 5;
        int volumes_no = 32;

        int pages_no = 410;
        int lines_no = 40;
        int simbs_no = 80;

        // Library of Babel Link base
        int LBL_base = 36;
    };
}