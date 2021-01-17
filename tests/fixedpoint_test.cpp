#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_NO_CPP11

#include "catch/catch.hpp"

#include <fixedpoint/fixedpoint.hpp>

using namespace fixed_point;

TEST_CASE("multiplication operator", "[scaled_int_4_4_t]") {

    typedef scaled_int_4_4_t::unscaled_int_type unscaled_int_4_4_t;
    typedef scaled_int_4_4_t::unscaled_float_type unscaled_float_4_4_t;

    SECTION("from float multiplication 0.375*0.5=0.1875") {
        scaled_int_4_4_t x(unscaled_float_4_4_t(0.375f));
        scaled_int_4_4_t y(unscaled_float_4_4_t(0.5f));

        REQUIRE(x * y == unscaled_float_4_4_t(0.1875f).scale());
    }

    SECTION("3*1 requires bigger intermediate storage to yield 3") {
        scaled_int_4_4_t x(unscaled_int_4_4_t(3));
        scaled_int_4_4_t y(unscaled_int_4_4_t(1));

        REQUIRE(x * y == unscaled_int_4_4_t(3).scale());
    }
}

TEST_CASE("division operator", "[scaled_int_4_4_t]") {

    typedef scaled_int_8_8_t::unscaled_int_type unscaled_int_8_8_t;

    scaled_int_8_8_t x(unscaled_int_8_8_t(10));
    scaled_int_8_8_t y(unscaled_int_8_8_t(5));

    REQUIRE(x / y == unscaled_int_8_8_t(2).scale());
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