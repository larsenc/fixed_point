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

TEST_CASE("scaling", "[scaled_int]") {

    SECTION("initialization from unscaled_int") {

        unscaled_int<3, 4> unscaled(5);
        scaled_int<3, 4> scaled = unscaled.scale();

        REQUIRE(scaled.getValue() == 80);

        SECTION("scaling back down to an int should without any changes should result in the same value as it was constructed from") {
            REQUIRE(scaled.unscaleToInt().value == 5);
        }
    }

    SECTION("initialization from unscaled_float") {

        unscaled_float<3, 4> unscaled(0.875f);
        scaled_int<3, 4> scaled = unscaled.scale();

        REQUIRE(scaled.getValue() == 14);

        SECTION("scaling back down to a float should without any changes should result in the same value as it was constructed from") {
            REQUIRE(scaled.unscaleToFloat().value == 0.875f);
        }
    }
}

TEST_CASE("multiplication", "[scaled_int]") {

    typedef scaled_int_3_4_t::unscaled_int_type unscaled_int_3_4_t;
    typedef scaled_int_7_8_t::unscaled_int_type unscaled_int_7_8_t;

    typedef scaled_int_3_4_t::unscaled_float_type unscaled_float_3_4_t;
    typedef scaled_int_7_8_t::unscaled_float_type unscaled_float_7_8_t;

    SECTION("bigger intermediate storage is required when multiplying 3.0 * 1.0 = 3.0") {
        scaled_int_3_4_t x(unscaled_int_3_4_t(3));
        scaled_int_3_4_t y(unscaled_int_3_4_t(1));

        REQUIRE(x * y == unscaled_int_7_8_t(3).scale());
    }

    SECTION("signed multiplication") {
        {
            scaled_int_3_4_t x(unscaled_int_3_4_t(3));
            scaled_int_3_4_t x_(unscaled_int_3_4_t(-3));

            scaled_int_3_4_t y(unscaled_int_3_4_t(1));
            scaled_int_3_4_t y_(unscaled_int_3_4_t(-1));

            REQUIRE(x * y == unscaled_int_7_8_t(3).scale());
            REQUIRE(x_ * y == unscaled_int_7_8_t(-3).scale());
            REQUIRE(x * y_ == unscaled_int_7_8_t(-3).scale());
            REQUIRE(x_ * y_ == unscaled_int_7_8_t(3).scale());
        }

        {
            scaled_int_3_4_t x(unscaled_float_3_4_t(0.375f));
            scaled_int_3_4_t x_(unscaled_float_3_4_t(-0.375f));

            scaled_int_3_4_t y(unscaled_float_3_4_t(0.5f));
            scaled_int_3_4_t y_(unscaled_float_3_4_t(-0.5f));

            REQUIRE(x * y == unscaled_float_7_8_t(0.1875f).scale());
            REQUIRE(x_ * y == unscaled_float_7_8_t(-0.1875f).scale());
            REQUIRE(x * y_ == unscaled_float_7_8_t(-0.1875f).scale());
            REQUIRE(x_ * y_ == unscaled_float_7_8_t(0.1875f).scale());
        }
    }

    SECTION("signed assignment multiplication") {
        scaled_int_3_4_t x(unscaled_float_3_4_t(1.3125f));

        x *= scaled_int_3_4_t(unscaled_float_3_4_t(0.625f));

        // Equals: 0.8203125, closes in Q3.4 is 0.8125
        REQUIRE(x == unscaled_float_3_4_t(0.8125f).scale());

        x *= scaled_int_3_4_t(unscaled_float_3_4_t(0.5f));

        // Equals: 0.40625, should be rounded up to 0.4375, closes value in Q3.4
        REQUIRE(x == unscaled_float_3_4_t(0.4375f).scale());
    }
}

TEST_CASE("division", "[scaled_int]") {

    typedef scaled_int_7_8_t::unscaled_int_type unscaled_int_7_8_t;
    typedef scaled_int_3_4_t::unscaled_float_type unscaled_float_3_4_t;
    typedef scaled_int_7_8_t::unscaled_float_type unscaled_float_7_8_t;

    SECTION("simple division 10.0 * 5.0 = 2.0") {
        scaled_int_7_8_t x(unscaled_int_7_8_t(10));
        scaled_int_7_8_t y(unscaled_int_7_8_t(5));

        REQUIRE(x / y == unscaled_int_7_8_t(2).scale());
    }


}

TEST_CASE("converting a Q7.8 to a Q3.4 with rounding", "[scaled_int]") {

    typedef scaled_int_3_4_t::unscaled_float_type unscaled_float_3_4_t;
    typedef scaled_int_7_8_t::unscaled_float_type unscaled_float_7_8_t;

    SECTION("Q7.8 : 2.03125f this is the halfway case and should be rounded away from zero when converting to Q3.4") {
        scaled_int_3_4_t a = scaled_int_7_8_t(unscaled_float_7_8_t(2.03125f)).convert<3, 4>();
        scaled_int_3_4_t b(unscaled_float_3_4_t(2.0625f));
        REQUIRE(a == b);
    }

    SECTION("Q7.8 : 2.02734375f is rounded down when converting to Q3.4") {
        scaled_int_3_4_t a = scaled_int_7_8_t(unscaled_float_7_8_t(2.02734375f)).convert<3, 4>();
        scaled_int_3_4_t b(unscaled_float_3_4_t(2.0f));
        REQUIRE(a == b);
    }

    SECTION("Q7.8 : 2.0390625 is rounded up when converting to Q3.4") {
        scaled_int_3_4_t a = scaled_int_7_8_t(unscaled_float_7_8_t(2.0390625f)).convert<3, 4>();
        scaled_int_3_4_t b(unscaled_float_3_4_t(2.0625f));
        REQUIRE(a == b);
    }
}

TEST_CASE("converting a negative Q7.8 to a Q3.4 with rounding", "[scaled_int]") {

        typedef scaled_int_3_4_t::unscaled_float_type unscaled_float_3_4_t;
        typedef scaled_int_7_8_t::unscaled_float_type unscaled_float_7_8_t;

    SECTION("Q7.8 : -2.03125f this is the halfway case and should be rounded away from zero when converting to Q3.4") {
        scaled_int_3_4_t a = scaled_int_7_8_t(unscaled_float_7_8_t(-2.03125f)).convert<3, 4>();
        scaled_int_3_4_t b(unscaled_float_3_4_t(-2.0625f));
        REQUIRE(a == b);
    }

    SECTION("Q7.8 : -2.02734375f is rounded up when converting to Q3.4") {
        scaled_int_3_4_t a = scaled_int_7_8_t(unscaled_float_7_8_t(-2.02734375f)).convert<3, 4>();
        scaled_int_3_4_t b(unscaled_float_3_4_t(-2.0f));
        REQUIRE(a == b);
    }

    SECTION("Q7.8 : -2.0390625 is rounded down when converting to Q3.4") {
        scaled_int_3_4_t a = scaled_int_7_8_t(unscaled_float_7_8_t(-2.0390625f)).convert<3, 4>();
        scaled_int_3_4_t b(unscaled_float_3_4_t(-2.0625f));
        REQUIRE(a == b);
    }
}


int main(int argc, char** argv)
{
    int result = Catch::Session().run(argc, argv);
    std::getchar();

    return result;
}