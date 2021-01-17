#ifndef FIXED_POINT_HPP
#define FIXED_POINT_HPP

namespace fixed_point {

	template<typename TStorage, uint8_t M, uint8_t N>
	class scaled_int;

	namespace detail {

		template<bool>
		struct verify_size;

		template<>
		struct verify_size<true>
		{};

		template<typename T>
		struct Intermediate;

		template<>
		struct Intermediate<uint8_t>
		{
			typedef uint16_t type;
		};

		template<>
		struct Intermediate<uint16_t>
		{
			typedef uint32_t type;
		};

		template<>
		struct Intermediate<uint32_t>
		{
			typedef uint64_t type;
		};

		template<>
		struct Intermediate<int8_t>
		{
			typedef int16_t type;
		};

		template<>
		struct Intermediate<int16_t>
		{
			typedef int32_t type;
		};

		template<>
		struct Intermediate<int32_t>
		{
			typedef int64_t type;
		};

	} // namespace detail

	template<typename TStorage, uint8_t M, uint8_t N>
	struct unscaled_int
	{
		explicit unscaled_int(const TStorage& value)
			: value(value)
		{
			detail::verify_size<M + N == 8 * sizeof(TStorage)>();
		}

		scaled_int<TStorage, M, N> scale() const
		{
			return scaled_int<TStorage, M, N>(value << N);
		}

		const TStorage value;
	};

#ifdef WITH_FLOAT_CONVERSION
	template<typename TStorage, uint8_t M, uint8_t N>
	struct unscaled_float
	{
		explicit unscaled_float(const float& value)
			: value(value)
		{
			detail::verify_size<M + N == 8 * sizeof(TStorage)>();
		}

		scaled_int<TStorage, M, N> scale() const
		{
			return scaled_int<TStorage, M, N>(static_cast<TStorage>(value * (1 << N)));
		}

		const float value;
	};
#endif

	/** unsigned integer:
	*                range: [0, 2^M - 2^(-N)]
	*           resolution: 2^(-N)
	*
	*   signed integer:
	*                range: [-(2^(M-1) - 2^(-N)), 2^(M-1) - 2^(-N)]
	*           resolution: 2^(-N)
	*/
	template<typename TStorage, uint8_t M, uint8_t N>
	class scaled_int
	{
	public:
		typedef scaled_int<TStorage, M, N>						scaled_int_type;
		typedef typename detail::Intermediate<TStorage>::type	intermediate_type;
		typedef unscaled_int<TStorage, M, N>				    unscaled_int_type;
#ifdef WITH_FLOAT_CONVERSION
		typedef unscaled_float<TStorage, M, N>				    unscaled_float_type;
#endif

		scaled_int()
			: mValue(0)
		{
			detail::verify_size<M + N == 8 * sizeof(TStorage)>();
		}

		scaled_int(const TStorage& scaledValue)
			: mValue(scaledValue)
		{
			detail::verify_size<M + N == 8 * sizeof(TStorage)>();
		}

		scaled_int(const scaled_int_type& scaledValue)
			: mValue(scaledValue.mValue)
		{}

		const TStorage& getValue() const
		{
			return mValue;
		}

		scaled_int(const unscaled_int_type& unscaled)
			: mValue(unscaled.scale().mValue)
		{}

		unscaled_int_type unscale() const
		{
			return unscaled_int_type(mValue / (1 << N));
		}

#ifdef WITH_FLOAT_CONVERSION
		scaled_int(const unscaled_float_type& unscaled)
			: mValue(unscaled.scale().mValue)
		{}

		unscaled_float_type unscale_to_float() const
		{
			return unscaled_float_type(static_cast<float>(mValue) / (1 << N));
		}
#endif

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
		TStorage mValue;
	};

	typedef scaled_int<int8_t, 4, 4> scaled_int_4_4_t;
	typedef scaled_int<int16_t, 8, 8> scaled_int_8_8_t;
	typedef scaled_int<int32_t, 16, 16> scaled_int_16_16_t;

} // namespace fixed_point


#endif // !FIXED_POINT_HPP