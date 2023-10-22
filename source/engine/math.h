#pragma once

#include "vector.h"

namespace dxlib {
namespace math {

template<class T>
inline constexpr T pi = T(3.14159265358979323846264338327950288);

template<class T = float>
inline constexpr T deg2rad(T deg)
{
	return deg * (pi<T> / 180.0f);
}

template<class T = float>
inline constexpr T rad2deg(T rad)
{
	return rad * (180.0f / pi<T>);
}

} // namespace math
} // namespace dxlib
