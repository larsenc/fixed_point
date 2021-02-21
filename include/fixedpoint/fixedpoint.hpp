#ifndef FIXED_POINT_HPP
#define FIXED_POINT_HPP

#include <stdint.h>

namespace fixed_point {

	template<uint8_t M, uint8_t N>
	class scaled_int;

	namespace detail {

		template<bool>
		struct static_assert_;

		template<>
		struct static_assert_<true>
		{};

		template<uint8_t TSize>
		struct storage;

		template<>
		struct storage<7>
		{
			typedef int8_t type;
		};

		template<>
		struct storage<15>
		{
			typedef int16_t type;
		};

		template<>
		struct storage<31>
		{
			typedef int32_t type;
		};

		template<uint8_t TSize>
		struct intermediate_storage;

		template<>
		struct intermediate_storage<7>
		{
			typedef int16_t type;
		};

		template<>
		struct intermediate_storage<15>
		{
			typedef int32_t type;
		};

		template<>
		struct intermediate_storage<31>
		{
			typedef int64_t type;
		};

		template<>
		struct intermediate_storage<63>
		{
			typedef int64_t type;
		};

		template<uint8_t M_OLD, uint8_t N_OLD, uint8_t M_NEW, uint8_t N_NEW, bool MORE_PRECISION>
		struct converter;

		template<uint8_t M_OLD, uint8_t N_OLD, uint8_t M_NEW, uint8_t N_NEW>
		struct converter<M_OLD, N_OLD, M_NEW, N_NEW, true>
		{
			typedef typename detail::storage<M_OLD + N_OLD>::type old_storage_type;
			typedef typename detail::storage<M_NEW + N_NEW>::type new_storage_type;

			// For the case N_OLD <= N_NEW we need to shift left.
			static scaled_int<M_NEW, N_NEW> convert(const scaled_int<M_OLD, N_OLD>& oldType)
			{
				detail::static_assert_<N_OLD <= N_NEW>();
				return scaled_int<M_NEW, N_NEW>(convert(oldType.getValue()));
			}

			static new_storage_type convert(const old_storage_type& oldValue)
			{
				detail::static_assert_<N_OLD <= N_NEW>();
				new_storage_type newValue = oldValue;
				newValue <<= -(N_OLD - N_NEW);
				return newValue;
			}
		};

		template<uint8_t M_OLD, uint8_t N_OLD, uint8_t M_NEW, uint8_t N_NEW>
		struct converter<M_OLD, N_OLD, M_NEW, N_NEW, false>
		{
			typedef typename detail::storage<M_OLD + N_OLD>::type old_storage_type;
			typedef typename detail::storage<M_NEW + N_NEW>::type new_storage_type;

			// For the case N_OLD > N_NEW we need to shift right and round
			static scaled_int<M_NEW, N_NEW> convert(const scaled_int<M_OLD, N_OLD>& oldType)
			{
				detail::static_assert_<(N_OLD > N_NEW)>();
				return scaled_int<M_NEW, N_NEW>(convert(oldType.getValue()));
			}

			static new_storage_type convert(old_storage_type value)
			{
				detail::static_assert_<(N_OLD > N_NEW)>();
				// Since we are decreasing the resolution, we need to round the old value.
				// We therefore add half the resolution of the old value before shifting right

				// Right shift of signed ints are compiler specific. I therefore want to use / instead of right shift.
				// TODO: Determine if there is a more efficient way when using /
				if (value < 0) {
					value -= (1 << (N_OLD - N_NEW - 1));
				}
				else {
					value += (1 << (N_OLD - N_NEW - 1));
				}
				value = value / (1 << (N_OLD - N_NEW));

				return static_cast<new_storage_type>(value);
			}
		};

	} // namespace detail

	template<uint8_t M, uint8_t N>
	struct unscaled_int
	{
		typedef typename detail::storage<M + N>::type storage_type;

		explicit unscaled_int(const storage_type& value)
			: value(value)
		{
			detail::static_assert_<M + N + 1== 8 * sizeof(storage_type)>();
		}

		scaled_int<M, N> scale() const
		{
			return scaled_int<M, N>(value << N);
		}

		const storage_type value;
	};

#ifdef WITH_FLOAT_CONVERSION
	template<uint8_t M, uint8_t N>
	struct unscaled_float
	{
		typedef typename detail::storage<M + N>::type storage_type;

		explicit unscaled_float(const float& value)
			: value(value)
		{
			detail::static_assert_<M + N + 1 == 8 * sizeof(storage_type)>();
		}

		scaled_int<M, N> scale() const
		{
			return scaled_int<M, N>(static_cast<storage_type>(value * (1 << N)));
		}

		const float value;
	};
#endif

	/**
	*  scaled_int uses the Q format where M is the number of xx bits and N is the number of fraction bits.
	*
	*        Range: [-(2^(M) - 2^(-N)), 2^(M) - 2^(-N)] NOTE: M+N = 7,15,31
	*   Resolution: 2^(-N)
	*
	* Implemented operators:
	*	Arthmetic:
	*		a + b
	*		a - b
	*		a * b
	*		a / b
	*	Comparison:
	*		a == b
	*		a != b
	*		a < b
	*		a > b
	*		a <= b
	*		a >= b
	*/
	template<uint8_t M, uint8_t N>
	class scaled_int
	{
	public:
		typedef scaled_int<M, N>									scaled_int_type;
		typedef typename detail::storage<M + N>::type				storage_type;
		typedef typename detail::intermediate_storage<M+N>::type	intermediate_type;
		typedef unscaled_int<M, N>									unscaled_int_type;
#ifdef WITH_FLOAT_CONVERSION
		typedef unscaled_float<M, N>								unscaled_float_type;
#endif

		enum { SCALE = 1 << N };

		scaled_int()
			: mValue(0)
		{
			detail::static_assert_<M + N + 1 == 8 * sizeof(storage_type)>();
		}

		explicit scaled_int(const storage_type& scaledValue)
			: mValue(scaledValue)
		{
			detail::static_assert_<M + N + 1 == 8 * sizeof(storage_type)>();
		}

		scaled_int(const scaled_int_type& scaledValue)
			: mValue(scaledValue.mValue)
		{}

		scaled_int(const unscaled_int_type& unscaled)
			: mValue(unscaled.scale().mValue)
		{}

#ifdef WITH_FLOAT_CONVERSION
		scaled_int(const unscaled_float_type& unscaled)
			: mValue(unscaled.scale().mValue)
		{}
#endif

		scaled_int_type& operator=(const scaled_int_type& rhs)
		{
			mValue = rhs.mValue;
			return *this;
		}

		const storage_type& getValue() const
		{
			return mValue;
		}

		unscaled_int_type unscaleToInt() const
		{
			return unscaled_int_type(mValue / SCALE);
		}

#ifdef WITH_FLOAT_CONVERSION
		unscaled_float_type unscaleToFloat() const
		{
			return unscaled_float_type(static_cast<float>(mValue) / SCALE);
		}
#endif

		/**
		*	Convert to another Q format.
		*/
		template<uint8_t M_NEW, uint8_t N_NEW>
		scaled_int<M_NEW, N_NEW> convert() const
		{
			return detail::converter<M, N, M_NEW, N_NEW, (N <= N_NEW)>::convert(*this);
		}

		/**
		*	Arthmetic assignment
		*/
		scaled_int_type& operator+=(const scaled_int_type& rhs)
		{
			mValue += rhs.mValue;
			return *this;
		}

		scaled_int_type& operator-=(const scaled_int_type& rhs)
		{
			mValue -= rhs.mValue;
			return *this;
		}

		scaled_int_type& operator/=(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue / rhs.mValue;
			mValue = intermediate << N;
			return *this;
		}

		scaled_int_type& operator*=(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue * rhs.mValue;
			mValue = detail::converter<2 * M + 1, 2 * N, M, N, (2 * N <= N)>::convert(intermediate);
			return *this;
		}

		/**
		*	Comparison operators
		*/
		bool operator==(const scaled_int_type& rhs) const
		{
			return mValue == rhs.mValue;
		}

		bool operator!=(const scaled_int_type& rhs) const
		{
			return mValue != rhs.mValue;
		}

		bool operator<(const scaled_int_type& rhs) const
		{
			return mValue < rhs.mValue;
		}

		bool operator>(const scaled_int_type& rhs) const
		{
			return mValue > rhs.mValue;
		}

		bool operator<=(const scaled_int_type& rhs) const
		{
			return mValue <= rhs.mValue;
		}

		bool operator>=(const scaled_int_type& rhs) const
		{
			return mValue >= rhs.mValue;
		}

	private:
		storage_type mValue;
	};

	/**
	*	Arthmetic
	*/
	template<uint8_t M, uint8_t N>
	scaled_int<M, N> operator+(const scaled_int<M, N>& lhs, const scaled_int<M, N>& rhs)
	{
		return scaled_int<M, N>(lhs.getValue() + rhs.getValue());
	}

	template<uint8_t M, uint8_t N>
	scaled_int<M, N> operator-(const scaled_int<M, N>& lhs, const scaled_int<M, N>& rhs)
	{
		return scaled_int<M, N>(lhs.getValue() - rhs.getValue());
	}

	template<uint8_t M, uint8_t N>
	scaled_int<M, N> operator/(const scaled_int<M, N>& lhs, const scaled_int<M, N>& rhs)
	{
		const typename scaled_int<M, N>::intermediate_type intermediate = lhs.getValue() / rhs.getValue();
		return scaled_int<M, N>(intermediate << N);
	}

	template<uint8_t M_LHS, uint8_t N_LHS, uint8_t M_RHS, uint8_t N_RHS>
	scaled_int<M_LHS + M_RHS + 1, N_LHS + N_RHS> operator*(const scaled_int<M_LHS, N_LHS>& lhs, const scaled_int<M_RHS, N_RHS>& rhs)
	{
		typedef scaled_int<M_LHS + M_RHS + 1, N_LHS + N_RHS> result_scaled_int_type;
		typename result_scaled_int_type::storage_type resultValue = lhs.getValue() * rhs.getValue();
		return result_scaled_int_type(resultValue);
	}

	typedef scaled_int<3, 4>	scaled_int_3_4_t;
	typedef scaled_int<7, 8>	scaled_int_7_8_t;
	typedef scaled_int<15, 16>	scaled_int_15_16_t;

} // namespace fixed_point

#endif // !FIXED_POINT_HPP