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
        const std::string charset = "abcdefghijklmnopqrstuvwxyz, .";

        static const int charset_no = 29;
        static const int walls_no = 4;
        static const int shelves_no = 5;
        static const int volumes_no = 32;

        static const int pages_no = 410;
        static const int lines_no = 40;
        static const int simbs_no = 80;

        // Library of Babel Link base
        static const int LBL_base = 36;
    };
}