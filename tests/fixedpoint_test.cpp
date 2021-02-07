#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_NO_CPP11

#include <fixedpoint/fixedpoint.hpp>

#include "catch/catch.hpp"

#include <ostream>
#include <stdio.h>

using namespace fixed_point;

namespace Catch {
    template<uint8_t M, uint8_t N>
    struct StringMaker<scaled_int<M, N> >
     {
        static std::string convert(const scaled_int<M, N>& scaledInt) {
            char str[20];
            const float v = scaledInt.unscaleToFloat().value;
            std::sprintf(str, "%f", v);
            return std::string(str);
        }
    };
}

TEST_CASE("multiplication", "[multiplication]") {

    typedef scaled_int_3_4_t::unscaled_int_type unscaled_int_3_4_t;
    typedef scaled_int_7_8_t::unscaled_int_type unscaled_int_7_8_t;

    typedef scaled_int_3_4_t::unscaled_float_type unscaled_float_3_4_t;
    typedef scaled_int_7_8_t::unscaled_float_type unscaled_float_7_8_t;

    SECTION("simple multiplication 0.375 * 0.5 = 0.1875") {
        scaled_int_3_4_t x(unscaled_float_3_4_t(0.375f));
        scaled_int_3_4_t y(unscaled_float_3_4_t(0.5f));

        REQUIRE(x * y == unscaled_float_7_8_t(0.1875f).scale());
    }

    SECTION("bigger intermediate storage is required when multiplying 3.0 * 1.0 = 3.0") {
        scaled_int_3_4_t x(unscaled_int_3_4_t(3));
        scaled_int_3_4_t y(unscaled_int_3_4_t(1));

        REQUIRE(x * y == unscaled_int_7_8_t(3).scale());
    }

    SECTION("signed multiplication") {
        scaled_int_3_4_t x(unscaled_int_3_4_t(3));
        scaled_int_3_4_t x_(unscaled_int_3_4_t(-3));

        scaled_int_3_4_t y(unscaled_int_3_4_t(1));
        scaled_int_3_4_t y_(unscaled_int_3_4_t(-1));

        REQUIRE(x_ * y == unscaled_int_7_8_t(-3).scale());
        REQUIRE(x * y_ == unscaled_int_7_8_t(-3).scale());
        REQUIRE(x_ * y_ == unscaled_int_7_8_t(3).scale());
    }
}

TEST_CASE("division", "[division]") {

    typedef scaled_int_7_8_t::unscaled_int_type unscaled_int_7_8_t;
    typedef scaled_int_3_4_t::unscaled_float_type unscaled_float_3_4_t;
    typedef scaled_int_7_8_t::unscaled_float_type unscaled_float_7_8_t;

    SECTION("simple division 10.0 * 5.0 = 2.0") {
        scaled_int_7_8_t x(unscaled_int_7_8_t(10));
        scaled_int_7_8_t y(unscaled_int_7_8_t(5));

        REQUIRE(x / y == unscaled_int_7_8_t(2).scale());
    }


}

TEST_CASE("rounding", "[scaled_int_4_4_t]") {
    // See part 3
}


int main(int argc, char** argv)
{
    int result = Catch::Session().run(argc, argv);
    std::getchar();

    return result;
}