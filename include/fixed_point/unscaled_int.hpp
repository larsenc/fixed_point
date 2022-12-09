#ifndef FIXED_POINT_UNSCALED_INT_HPP
#define FIXED_POINT_UNSCALED_INT_HPP

#include "scaled_int.hpp"
#include "storage_type.hpp"
#include "type_traits.hpp"

#include <stdint.h>

namespace fixed_point {

template<uint8_t TM, uint8_t TN>
struct unscaled_int
{
	enum {
		M = TM,
		N = TN
	};

	typedef typename detail::storage<M + N>::type	storage_type;
	typedef storage_type							value_type;

	explicit unscaled_int(value_type value)
		: value(value)
	{
		detail::static_assert_<M + N + 1 == 8 * sizeof(storage_type)>();
	}

	scaled_int<M, N> scale() const
	{
		return scaled_int<M, N>(static_cast<storage_type>(value << N));
	}

	const value_type value;
};

template<uint8_t TM, uint8_t TN>
struct unscaled_float
{
	enum {
		M = TM,
		N = TN
	};

	typedef typename detail::storage<M + N>::type	storage_type;
	typedef float									value_type;

	explicit unscaled_float(value_type value)
		: value(value)
	{
		detail::static_assert_<M + N + 1 == 8 * sizeof(storage_type)>();
	}

	scaled_int<M, N> scale() const
	{
		return scaled_int<M, N>(static_cast<storage_type>(value * (1 << N)));
	}

	const value_type value;
};

template<uint8_t TM, uint8_t TN>
struct unscaled_double
{
	enum {
		M = TM,
		N = TN
	};

	typedef typename detail::storage<M + N>::type	storage_type;
	typedef double									value_type;

	explicit unscaled_double(value_type value)
		: value(value)
	{
		detail::static_assert_<M + N + 1 == 8 * sizeof(storage_type)>();
	}

	scaled_int<M, N> scale() const
	{
		return scaled_int<M, N>(static_cast<storage_type>(value * (1 << N)));
	}

	const value_type value;
};

} // namespace fixed_point

#endif // FIXED_POINT_UNSCALED_INT_HPP