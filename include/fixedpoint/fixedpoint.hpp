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
			typedef typename detail::storage<M_NEW + N_NEW>::type new_storage_type;

			// For the case N_OLD <= N_NEW we need to shift left.
			static scaled_int<M_NEW, N_NEW> convert(const scaled_int<M_OLD, N_OLD>& old)
			{
				detail::static_assert_<N_OLD <= N_NEW>();
				new_storage_type newValue = old.getValue();
				newValue <<= -(N_OLD - N_NEW);
				return scaled_int<M_NEW, N_NEW>(newValue);
			}
		};

		template<uint8_t M_OLD, uint8_t N_OLD, uint8_t M_NEW, uint8_t N_NEW>
		struct converter<M_OLD, N_OLD, M_NEW, N_NEW, false>
		{
			typedef typename detail::storage<M_OLD + N_OLD>::type old_storage_type;
			typedef typename detail::storage<M_NEW + N_NEW>::type new_storage_type;

			// For the case N_OLD > N_NEW we need to shift right and round
			static scaled_int<M_NEW, N_NEW> convert(const scaled_int<M_OLD, N_OLD>& old)
			{
				detail::static_assert_<(N_OLD > N_NEW)>();
				// Since we are decreasing the resolution, we need to round the old value.
				// We therefore add half the resolution of the old value before shifting right
				old_storage_type newValue = old.getValue();


				// Right shift of signed ints are compiler specific. I therefore want to use / instead of right shift.
				// TODO: Determine if there is a more efficient way when using /
				if (newValue < 0) {
					newValue -= (1 << (N_OLD - N_NEW - 1));
				}
				else {
					newValue += (1 << (N_OLD - N_NEW - 1));

				}
				newValue = newValue / (1 << (N_OLD - N_NEW));

				return scaled_int<M_NEW, N_NEW>(static_cast<new_storage_type>(newValue));
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
	*   signed integer:
	*                range: [-(2^(M) - 2^(-N)), 2^(M) - 2^(-N)] NOTE: M+N = 7,15,31
	*           resolution: 2^(-N)
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

		const storage_type& getValue() const
		{
			return mValue;
		}

		scaled_int(const unscaled_int_type& unscaled)
			: mValue(unscaled.scale().mValue)
		{}

		unscaled_int_type unscaleToInt() const
		{
			return unscaled_int_type(mValue / (1 << N));
		}

#ifdef WITH_FLOAT_CONVERSION
		scaled_int(const unscaled_float_type& unscaled)
			: mValue(unscaled.scale().mValue)
		{}

		unscaled_float_type unscaleToFloat() const
		{
			return unscaled_float_type(static_cast<float>(mValue) / (1 << N));
		}
#endif

		template<uint8_t M_NEW, uint8_t N_NEW>
		scaled_int<M_NEW, N_NEW> convert() const
		{
			return detail::converter<M, N, M_NEW, N_NEW, (N <= N_NEW)>::convert(*this);
		}

		/** Arithmetic https://en.cppreference.com/w/cpp/language/operator_arithmetic
		* a + b
		* a - b
		* a * b
		* a / b
		*/
		scaled_int_type operator+(const scaled_int_type& rhs)
		{
			return scaled_int_type(mValue + rhs.mValue);
		}

		scaled_int_type operator-(const scaled_int_type& rhs)
		{
			return scaled_int_type(mValue - rhs.mValue);
		}

		scaled_int_type operator/(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue / rhs.mValue;
			return scaled_int_type(intermediate << N);
		}

		scaled_int_type operator*(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue * rhs.mValue;
			return scaled_int_type(intermediate >> N);
		}

		/** Comparison
		* a == b
		* a != b
		* a < b
		* a > b
		* a <= b
		* a >= b
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

		/** Assignment
		* a = b
		* a += b
		* a -= b
		* a *= b
		* a /= b
		*/
		scaled_int_type& operator=(const scaled_int_type& rhs)
		{
			mValue = rhs.mValue;
			return *this;
		}

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

		scaled_int_type& operator*=(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue * rhs.mValue;
			mValue = intermediate >> N;
			return *this;
		}

		scaled_int_type& operator/=(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue / rhs.mValue;
			mValue = intermediate << N;
			return *this;
		}

	private:
		storage_type mValue;
	};

	typedef scaled_int<3, 4> scaled_int_3_4_t;
	typedef scaled_int<7, 8> scaled_int_7_8_t;
	typedef scaled_int<15, 16> scaled_int_15_16_t;

} // namespace fixed_point


#endif // !FIXED_POINT_HPP