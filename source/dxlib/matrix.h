#pragma once

#include "vector.h"

namespace dxlib {
namespace math {

template<typename T>
struct mat2
{
	T _00, _01;
	T _10, _11;
};

template<typename T>
struct mat3
{
	T _00, _01, _02;
	T _10, _11, _12;
	T _20, _21, _22;
};

template<typename T>
struct mat4
{
	T _00, _01, _02, _03;
	T _10, _11, _12, _13;
	T _20, _21, _22, _23;
	T _30, _31, _32, _33;
};

} // namespace math
} // namespace dxlib

using float2x2 = dxlib::math::mat2<float>;
using float3x3 = dxlib::math::mat3<float>;
using float4x4 = dxlib::math::mat4<float>;
