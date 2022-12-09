#ifndef FIXED_POINT_STORAGE_TYPE_HPP
#define FIXED_POINT_STORAGE_TYPE_HPP

#include <stdint.h>

namespace fixed_point {
namespace detail {

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

} // namespace detail
} // namespace fixed_point

#endif // FIXED_POINT_STORAGE_TYPE_HPP