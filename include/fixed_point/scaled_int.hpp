#ifndef SCALED_INT_HPP
#define SCALED_INT_HPP

#include <stdint.h>

namespace fixed_point {

	template<uint8_t TM, uint8_t TN>
	class scaled_int;

	namespace detail {

		template<bool>
		struct static_assert_;

		template <typename T>
		struct is_floating_point { enum { value = 0 }; };

		template <>
		struct is_floating_point<float> { enum { value = 1 }; };

		template <>
		struct is_floating_point<double> { enum { value = 1 }; };

		template <>
		struct is_floating_point<long double> { enum { value = 1 }; };

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

		template<>
		struct storage<63>
		{
			typedef int64_t type;
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
				// TODO: Determine if there is a more efficient way when using division(/ operator).
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

	/**
	*	scaled_int implements a fixed point number. It uses the QM.N format where M is the number of
	*	integer bits and N is the number of fraction bits. The floating point number scaled_int
	*	represents has the following range and resolution.
	*
	*			 Range: [-(2^(M)), 2^(M) - 2^(-N)] where M+N = 7,15,31,63
	*		Resolution: 2^(-N)
	*
	*	Implemented operators:
	*		Arthmetic:
	*			a + b
	*			a - b
	*			a * b
	*			a / b
	*		Comparison:
	*			a == b
	*			a != b
	*			a < b
	*			a > b
	*			a <= b
	*			a >= b
	*
	*	Note: Only subtraction and addition operators available when M+N=63
	*
	*/
	template<uint8_t TM, uint8_t TN>
	class scaled_int
	{
	public:
		enum {
			M = TM,
			N = TN,
			BIT_SIZE = TM + TN + 1
		};

		typedef scaled_int<M, N>									scaled_int_type;
		typedef typename detail::storage<M + N>::type				storage_type;
		typedef typename detail::intermediate_storage<M+N>::type	intermediate_type;

		scaled_int()
			: mValue(0)
		{
			detail::static_assert_<BIT_SIZE == 8 * sizeof(storage_type)>();
		}

		explicit scaled_int(const storage_type& scaledValue)
			: mValue(scaledValue)
		{
			detail::static_assert_<BIT_SIZE == 8 * sizeof(storage_type)>();
		}

		scaled_int(const scaled_int_type& scaledValue)
			: mValue(scaledValue.mValue)
		{}

		template<typename TUnscaled>
		scaled_int(const TUnscaled& unscaled)
			: mValue(unscaled.scale().mValue)
		{
			detail::static_assert_<(int)M == (int)TUnscaled::M>();
			detail::static_assert_<(int)N == (int)TUnscaled::N>();
		}

		scaled_int_type& operator=(const scaled_int_type& rhs)
		{
			mValue = rhs.mValue;
			return *this;
		}

		const storage_type& getValue() const
		{
			return mValue;
		}

		template<typename TReturnType>
		TReturnType unscale() const
		{
			detail::static_assert_<detail::is_floating_point<TReturnType>::value || sizeof(TReturnType) >= sizeof(storage_type)>();
			return static_cast<TReturnType>(mValue) / (storage_type(1) << N);
		}

		/**
		*	Convert to another Q format.
		*/
		template<uint8_t M_NEW, uint8_t N_NEW>
		scaled_int<M_NEW, N_NEW> convert() const
		{
			return detail::converter<M, N, M_NEW, N_NEW, (N <= N_NEW)>::convert(*this);
		}

		/**
		*	Arthmetic assignment operators
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
			const intermediate_type intermediate = ((static_cast<intermediate_type>(mValue) << (M + N)) / rhs.getValue());
			mValue = detail::converter<M + N + 1, M + N, M, N, (M + N <= N)>::convert(intermediate);
			return *this;
		}

		scaled_int_type& operator*=(const scaled_int_type& rhs)
		{
			const intermediate_type intermediate = mValue * rhs.mValue;
			mValue = detail::converter<2 * M + 1, 2 * N, M, N, (2 * N <= N)>::convert(intermediate);
			return *this;
		}

		/**
		*	Increment/decrement prefix operators
		*/
		scaled_int_type& operator++()
		{
			++mValue;
			return *this;
		}

		scaled_int_type& operator--()
		{
			--mValue;
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
	*	Arthmetic operators
	*/
	template<uint8_t M, uint8_t N>
	scaled_int<M, N> operator+(const scaled_int<M, N>& lhs, const scaled_int<M, N>& rhs)
	{
		typedef typename scaled_int<M, N>::storage_type storage_type;
		return scaled_int<M, N>(static_cast<storage_type>(lhs.getValue() + rhs.getValue()));
	}

	template<uint8_t M, uint8_t N>
	scaled_int<M, N> operator-(const scaled_int<M, N>& lhs, const scaled_int<M, N>& rhs)
	{
		typedef typename scaled_int<M, N>::storage_type storage_type;
		return scaled_int<M, N>(static_cast<storage_type>(lhs.getValue() - rhs.getValue()));
	}

	template<uint8_t M_LHS, uint8_t N_LHS, uint8_t M_RHS, uint8_t N_RHS>
	scaled_int<M_LHS + N_RHS + 1, M_RHS + N_LHS> operator/(const scaled_int<M_LHS, N_LHS>& lhs, const scaled_int<M_RHS, N_RHS>& rhs)
	{
		typedef scaled_int<M_LHS + N_RHS + 1, M_RHS + N_LHS> result_scaled_int_type;
		typedef typename result_scaled_int_type::storage_type result_storage_type;
		result_storage_type resultValue = lhs.getValue();
		return result_scaled_int_type(static_cast<result_storage_type>((resultValue << (M_RHS + N_RHS)) / rhs.getValue()));
	}

	template<uint8_t M_LHS, uint8_t N_LHS, uint8_t M_RHS, uint8_t N_RHS>
	scaled_int<M_LHS + M_RHS + 1, N_LHS + N_RHS> operator*(const scaled_int<M_LHS, N_LHS>& lhs, const scaled_int<M_RHS, N_RHS>& rhs)
	{
		typedef scaled_int<M_LHS + M_RHS + 1, N_LHS + N_RHS> result_scaled_int_type;
		typedef typename result_scaled_int_type::storage_type result_storage_type;
		const result_storage_type resultValue = static_cast<result_storage_type>(lhs.getValue()) * rhs.getValue();
		return result_scaled_int_type(resultValue);
	}
} // namespace fixed_point

#endif // !SCALED_INT_HPP
