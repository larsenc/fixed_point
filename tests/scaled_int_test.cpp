#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_NO_CPP11

#include <fixed_point/scaled_int.hpp>

#include "catch/catch.hpp"

using namespace fixed_point;

typedef scaled_int<3, 4>	scaled_int_3_4_t;
typedef scaled_int<7, 8>	scaled_int_7_8_t;
typedef scaled_int<15, 16>	scaled_int_15_16_t;

typedef unscaled_float<3, 4>    unscaled_float_3_4_t;
typedef unscaled_float<7, 8>    unscaled_float_7_8_t;
typedef unscaled_float<15, 16>  unscaled_float_15_16_t;
typedef unscaled_float<16, 15>  unscaled_float_16_15_t;

typedef unscaled_int<3, 4>      unscaled_int_3_4_t;
typedef unscaled_int<7, 8>      unscaled_int_7_8_t;
typedef unscaled_int<16, 15>    unscaled_int_16_15_t;

namespace Catch {
    template<uint8_t M, uint8_t N>
    struct StringMaker<scaled_int<M, N> >
     {
        static std::string convert(const scaled_int<M, N>& scaledInt) {
            typedef unscaled_double<M, N> unscaled_double_M_N_t;
            char str[20];
            const double v = scaledInt.template unscale<double>();
            std::sprintf(str, "%f", v);
            return std::string(str);
        }
    };
}

TEST_CASE("scaling", "[scaled_int]") {

    SECTION("initialization from unscaled_int") {

        unscaled_float_3_4_t unscaled(5);
        scaled_int_3_4_t scaled = unscaled.scale();

        REQUIRE(scaled.getValue() == 80);

        SECTION("scaling back down to an int should, without any changes, result in the same value it was constructed from") {
            REQUIRE(scaled.unscale<int>() == 5);
        }
    }

    SECTION("initialization from unscaled_float") {

        unscaled_float_3_4_t unscaled(0.875f);
        scaled_int_3_4_t scaled = unscaled.scale();

        REQUIRE(scaled.getValue() == 14);

        SECTION("scaling back down to a float should, without any changes, result in the same value it was constructed from") {
            REQUIRE(scaled.unscale<float>() == 0.875f);
        }

        SECTION("scaling down to a double should, without any changes, result in the same value it was constructed from") {
            REQUIRE(scaled.unscale<double>() == 0.875);
        }
    }
}

TEST_CASE("addition", "[scaled_int]") {
    SECTION("signed addition") {
        const scaled_int_15_16_t x(unscaled_float_15_16_t(1.5f));
        const scaled_int_15_16_t x_(unscaled_float_15_16_t(-1.5f));

        const scaled_int_15_16_t y(unscaled_float_15_16_t(4.25f));
        const scaled_int_15_16_t y_(unscaled_float_15_16_t(-4.25f));

        REQUIRE(x + y == unscaled_float_15_16_t(5.75f).scale());
        REQUIRE(x_ + y == unscaled_float_15_16_t(2.75f).scale());
        REQUIRE(x + y_ == unscaled_float_15_16_t(-2.75f).scale());
        REQUIRE(x_ + y_ == unscaled_float_15_16_t(-5.75).scale());
    }

    SECTION("signed addition assignment") {
        scaled_int_15_16_t x(unscaled_float_15_16_t(7.0f));

        x += scaled_int_15_16_t(unscaled_float_15_16_t(6.0f));
        REQUIRE(x == unscaled_float_15_16_t(13.0f).scale());

        x += scaled_int_15_16_t(unscaled_float_15_16_t(-1.75f));
        REQUIRE(x == unscaled_float_15_16_t(11.25f).scale());
    }
}

TEST_CASE("subtraction", "[scaled_int]") {
    SECTION("signed subtraction") {
        const scaled_int_15_16_t x(unscaled_float_15_16_t(1.5f));
        const scaled_int_15_16_t x_(unscaled_float_15_16_t(-1.5f));

        const scaled_int_15_16_t y(unscaled_float_15_16_t(4.25f));
        const scaled_int_15_16_t y_(unscaled_float_15_16_t(-4.25f));

        REQUIRE(x - y == unscaled_float_15_16_t(-2.75f).scale());
        REQUIRE(x_ - y == unscaled_float_15_16_t(-5.75f).scale());
        REQUIRE(x - y_ == unscaled_float_15_16_t(5.75f).scale());
        REQUIRE(x_ - y_ == unscaled_float_15_16_t(2.75).scale());
    }

    SECTION("signed subtraction assignment") {
        scaled_int_15_16_t x(unscaled_float_15_16_t(7.0f));

        x -= scaled_int_15_16_t(unscaled_float_15_16_t(6.0f));
        REQUIRE(x == unscaled_float_15_16_t(1.0f).scale());

        x -= scaled_int_15_16_t(unscaled_float_15_16_t(-1.75f));
        REQUIRE(x == unscaled_float_15_16_t(2.75f).scale());
    }
}

TEST_CASE("multiplication", "[scaled_int]") {
    SECTION("bigger intermediate storage") {
        {
            scaled_int_3_4_t x(unscaled_int_3_4_t(3));
            scaled_int_3_4_t y(unscaled_int_3_4_t(1));

            REQUIRE(x * y == unscaled_int_7_8_t(3).scale());
        }
        {
            scaled_int<12, 19> x(52428);
            scaled_int<15, 16> y(3276800);
            scaled_int<28, 35> z(171796070400);
            REQUIRE(x * y == z);
        }
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

        // Equals: 0.8203125, closest in Q3.4 is 0.8125
        REQUIRE(x == unscaled_float_3_4_t(0.8125f).scale());

        x *= scaled_int_3_4_t(unscaled_float_3_4_t(0.5f));

        // Equals: 0.40625, closest in Q3.4 is 0.4375
        REQUIRE(x == unscaled_float_3_4_t(0.4375f).scale());
    }
}

TEST_CASE("division", "[scaled_int]") {
    SECTION("simple division 10.0 / 5.0 = 2.0") {
        scaled_int_7_8_t x(unscaled_int_7_8_t(10));
        scaled_int_7_8_t y(unscaled_int_7_8_t(5));

        REQUIRE(x / y == unscaled_int_16_15_t(2).scale());
    }

    SECTION("signed division") {
        scaled_int_7_8_t x(unscaled_float_7_8_t(0.375f));
        scaled_int_7_8_t x_(unscaled_float_7_8_t(-0.375f));

        scaled_int_7_8_t y(unscaled_float_7_8_t(0.5f));
        scaled_int_7_8_t y_(unscaled_float_7_8_t(-0.5f));

        REQUIRE(x / y == unscaled_float_16_15_t(0.75f).scale());
        REQUIRE(x / y_ == unscaled_float_16_15_t(-0.75f).scale());
        REQUIRE(x_ / y == unscaled_float_16_15_t(-0.75f).scale());
        REQUIRE(x_ / y_ == unscaled_float_16_15_t(0.75f).scale());

        REQUIRE(y / x == unscaled_float_16_15_t(1.3333333333333333333333333f).scale());
        REQUIRE(y / x_ == unscaled_float_16_15_t(-1.3333333333333333333333333f).scale());
        REQUIRE(y_ / x == unscaled_float_16_15_t(-1.3333333333333333333333333f).scale());
        REQUIRE(y_ / x_ == unscaled_float_16_15_t(1.3333333333333333333333333f).scale());
    }

    SECTION("signed assignment division") {
        scaled_int_3_4_t x(unscaled_float_3_4_t(1.3125f));

        x /= scaled_int_3_4_t(unscaled_float_3_4_t(0.625f));

        // Equals: 2.1, closes in Q3.4 is 2.125
        REQUIRE(x == unscaled_float_3_4_t(2.125f).scale());

        x /= scaled_int_3_4_t(unscaled_float_3_4_t(-2.0f));

        REQUIRE(x == unscaled_float_3_4_t(-1.0625f).scale());
    }
}

TEST_CASE("converting a Q7.8 to a Q3.4 with rounding", "[scaled_int]") {
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