#include <fixed_point/scaled_int.hpp>
#include <fixed_point/unscaled_int.hpp>

#include <iostream>

int main() {
	using namespace fixed_point;

	typedef scaled_int<7, 8> scaled_int_7_8_t;
	typedef scaled_int<15, 16> scaled_int_15_16_t;

	typedef unscaled_float<7, 8> unscaled_float_7_8_t;

	scaled_int_7_8_t x(unscaled_float_7_8_t(0.125f));
	scaled_int_7_8_t y(unscaled_float_7_8_t(2.0f));

	scaled_int_15_16_t z = x * y;

	scaled_int_7_8_t q(z.convert<7, 8>());

	std::cout << "0.125 * 2.0 = " << q.unscale<float>() << std::endl;

	return 0;
}