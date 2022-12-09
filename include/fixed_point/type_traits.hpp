#ifndef FIXED_POINT_TYPE_TRAITS_HPP
#define FIXED_POINT_TYPE_TRAITS_HPP

namespace fixed_point {
namespace detail {

template<bool>
struct static_assert_;

template<>
struct static_assert_<true>
{};

template <typename T>
struct is_floating_point { enum { value = 0 }; };

template <>
struct is_floating_point<float> { enum { value = 1 }; };

template <>
struct is_floating_point<double> { enum { value = 1 }; };

template <>
struct is_floating_point<long double> { enum { value = 1 }; };

} // namespace detail
} // namespace fixed_point

#endif // FIXED_POINT_TYPE_TRAITS_HPP