#pragma once

#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
#include <DirectXMath.h>
#endif

#include <cstdint>
#include <type_traits>

namespace dxlib {
namespace math {
namespace detail {

template<class T>
struct vec2_traits;

template<>
struct vec2_traits<int32_t>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMINT2;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<>
struct vec2_traits<uint32_t>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMUINT2;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<>
struct vec2_traits<float>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMFLOAT2;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<class T>
struct vec3_traits;

template<>
struct vec3_traits<int32_t>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMINT3;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<>
struct vec3_traits<uint32_t>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMUINT3;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<>
struct vec3_traits<float>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMFLOAT3;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<class T>
struct vec4_traits;

template<>
struct vec4_traits<int32_t>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMINT4;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<>
struct vec4_traits<uint32_t>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMUINT4;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

template<>
struct vec4_traits<float>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using xm_type  = DirectX::XMFLOAT4;
	using xmv_type = DirectX::XMVECTOR;
#endif
};

} // namespace detail

template<class T>
struct vec2 : public detail::vec2_traits<T>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using traits   = detail::vec2_traits<T>;
	using xm_type  = traits::xm_type;
	using xmv_type = traits::xmv_type;
#endif

	T x, y;

	vec2() = default;

	vec2(const vec2&) = default;

	vec2(vec2&&) = default;

	constexpr vec2(T x, T y) noexcept
	    : x(x)
	    , y(y)
	{
	}

	constexpr vec2(T v) noexcept
	    : x(v)
	    , y(v)
	{
	}

	template<class X, class Y>
	constexpr vec2(X x, Y y) noexcept
	    : x(static_cast<T>(x))
	    , y(static_cast<T>(y))
	{
	}

	template<class U>
	constexpr vec2(const vec2<U>& v) noexcept
	    : x(static_cast<T>(v.x))
	    , y(static_cast<T>(v.y))
	{
	}

	vec2& operator=(const vec2&) = default;

	vec2& operator=(vec2&&) = default;

	[[nodiscard]] constexpr bool operator==(const vec2& v) const noexcept
	{
		return x == v.x && y == v.y;
	}

	[[nodiscard]] constexpr bool operator!=(const vec2& v) const noexcept
	{
		return x != v.x || y != v.y;
	}

	[[nodiscard]] constexpr vec2 operator+() const noexcept
	{
		return *this;
	}

	[[nodiscard]] constexpr vec2 operator-() const noexcept
	{
		return { -x, -y };
	}

	[[nodiscard]] constexpr vec2 operator+(const vec2& v) const noexcept
	{
		return { x + v.x, y + v.y };
	}

	[[nodiscard]] constexpr vec2 operator-(const vec2& v) const noexcept
	{
		return { x - v.x, y - v.y };
	}

	[[nodiscard]] constexpr vec2 operator*(const vec2& v) const noexcept
	{
		return { x * v.x, y * v.y };
	}

	[[nodiscard]] constexpr vec2 operator/(const vec2& v) const noexcept
	{
		return { x / v.x, y / v.y };
	}

	vec2 operator+=(const vec2& v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	vec2 operator-=(const vec2& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2 operator*=(const vec2& v) noexcept
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	vec2 operator/=(const vec2& v) noexcept
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	vec2 operator=(const T& v) noexcept
	{
		x = v;
		y = v;
		return *this;
	}

	vec2 operator+=(const T& v) noexcept
	{
		x += v;
		y += v;
		return *this;
	}

	vec2 operator-=(const T& v) noexcept
	{
		x -= v;
		y -= v;
		return *this;
	}

	vec2 operator*=(const T& v) noexcept
	{
		x *= v;
		y *= v;
		return *this;
	}

	vec2 operator/=(const T& v) noexcept
	{
		x /= v;
		y /= v;
		return *this;
	}

#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	// --- xm_type helper ---

	vec2(const xm_type& v) noexcept
	    : x(v.x)
	    , y(v.y)
	{
	}

	operator xm_type() const noexcept
	{
		return xm_type(x, y);
	}

	// --- xmv_type helper ---

	vec2(const xmv_type& v) noexcept
	{
		if constexpr (std::is_same_v<xm_type, DirectX::XMINT2>) {
			DirectX::XMStoreSInt2(reinterpret_cast<xm_type*>(this), v);
		}
		else if constexpr (std::is_same_v<xm_type, DirectX::XMUINT2>) {
			DirectX::XMStoreUInt2(reinterpret_cast<xm_type*>(this), v);
		}
		else {
			DirectX::XMStoreFloat2(reinterpret_cast<xm_type*>(this), v);
		}
	}

	operator xmv_type() const noexcept
	{
		if constexpr (std::is_same_v<xm_type, DirectX::XMINT2>) {
			return DirectX::XMLoadSInt2(reinterpret_cast<const xm_type*>(this));
		}
		else if constexpr (std::is_same_v<xm_type, DirectX::XMUINT2>) {
			return DirectX::XMLoadUInt2(reinterpret_cast<const xm_type*>(this));
		}
		return DirectX::XMLoadFloat2(reinterpret_cast<const xm_type*>(this));
	}
#endif
};

template<class T>
struct vec3 : public detail::vec3_traits<T>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using traits   = detail::vec3_traits<T>;
	using xm_type  = traits::xm_type;
	using xmv_type = traits::xmv_type;
#endif

	T x, y, z;

	vec3() = default;

	vec3(const vec3&) = default;

	vec3(vec3&&) = default;

	constexpr vec3(T x, T y, T z) noexcept
	    : x(x)
	    , y(y)
	    , z(z)
	{
	}

	constexpr vec3(T v) noexcept
	    : x(v)
	    , y(v)
	    , z(v)
	{
	}

	template<class X, class Y, class Z>
	constexpr vec3(X x, Y y, Z z) noexcept
	    : x(static_cast<T>(x))
	    , y(static_cast<T>(y))
	    , z(static_cast<T>(z))
	{
	}

	template<class U>
	constexpr vec3(const vec3<U>& v) noexcept
	    : x(static_cast<T>(v.x))
	    , y(static_cast<T>(v.y))
	    , z(static_cast<T>(v.z))
	{
	}

	template<class Z>
	constexpr vec3(const vec2<T>& v, Z z) noexcept
	    : x(v.x)
	    , y(v.y)
	    , z(static_cast<T>(z))
	{
	}

	template<class U, class Z>
	constexpr vec3(const vec2<U>& v, Z z) noexcept
	    : x(static_cast<T>(v.x))
	    , y(static_cast<T>(v.y))
	    , z(static_cast<T>(z))
	{
	}

	vec3& operator=(const vec3&) = default;

	vec3& operator=(vec3&&) = default;

	[[nodiscard]] constexpr bool operator==(const vec3& v) const noexcept
	{
		return x == v.x && y == v.y && z == v.z;
	}

	[[nodiscard]] constexpr bool operator!=(const vec3& v) const noexcept
	{
		return x != v.x && y != v.y && z != v.z;
	}

	[[nodiscard]] constexpr vec3 operator+(const vec3& v) const noexcept
	{
		return { x + v.x, y + v.y, z + v.z };
	}

	[[nodiscard]] constexpr vec3 operator-(const vec3& v) const noexcept
	{
		return { x - v.x, y - v.y, z - v.z };
	}

	[[nodiscard]] constexpr vec3 operator*(const vec3& v) const noexcept
	{
		return { x * v.x, y * v.y, z * v.z };
	}

	[[nodiscard]] constexpr vec3 operator/(const vec3& v) const noexcept
	{
		return { x / v.x, y / v.y, z / v.z };
	}

	vec3 operator+=(const vec3& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3 operator-=(const vec3& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3 operator*=(const vec3& v) noexcept
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vec3 operator/=(const vec3& v) noexcept
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	vec3 operator=(const T& v) noexcept
	{
		x = v;
		y = v;
		z = v;
		return *this;
	}

	vec3 operator+=(const T& v) noexcept
	{
		x += v;
		y += v;
		z += v;
		return *this;
	}

	vec3 operator-=(const T& v) noexcept
	{
		x -= v;
		y -= v;
		z -= v;
		return *this;
	}

	vec3 operator*=(const T& v) noexcept
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3 operator/=(const T& v) noexcept
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	// --- xm_type helper ---

	vec3(const xm_type& v) noexcept
	    : x(v.x)
	    , y(v.y)
	    , z(v.z)
	{
	}

	operator xm_type() const noexcept
	{
		return xm_type(x, y, z);
	}

	// --- xmv_type helper ---

	vec3(const xmv_type& v) noexcept
	{
		if constexpr (std::is_same_v<xm_type, DirectX::XMINT3>) {
			DirectX::XMStoreSInt3(reinterpret_cast<xm_type*>(this), v);
		}
		else if constexpr (std::is_same_v<xm_type, DirectX::XMUINT3>) {
			DirectX::XMStoreUInt3(reinterpret_cast<xm_type*>(this), v);
		}
		else {
			DirectX::XMStoreFloat3(reinterpret_cast<xm_type*>(this), v);
		}
	}

	operator xmv_type() const noexcept
	{
		if constexpr (std::is_same_v<xm_type, DirectX::XMINT2>) {
			return DirectX::XMLoadSInt3(reinterpret_cast<const xm_type*>(this));
		}
		else if constexpr (std::is_same_v<xm_type, DirectX::XMUINT2>) {
			return DirectX::XMLoadUInt3(reinterpret_cast<const xm_type*>(this));
		}
		return DirectX::XMLoadFloat3(reinterpret_cast<const xm_type*>(this));
	}
#endif
};

template<class T>
struct vec4 : public detail::vec4_traits<T>
{
#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	using traits   = detail::vec4_traits<T>;
	using xm_type  = traits::xm_type;
	using xmv_type = traits::xmv_type;
#endif

	T x, y, z, w;

	vec4() = default;

	vec4(const vec4&) = default;

	vec4(vec4&&) = default;

	constexpr vec4(T x, T y, T z, T w) noexcept
	    : x(x)
	    , y(y)
	    , z(z)
	    , w(w)
	{
	}

	constexpr vec4(T v) noexcept
	    : x(v)
	    , y(v)
	    , z(v)
	    , w(v)
	{
	}

	template<class X, class Y, class Z, class W>
	constexpr vec4(X x, Y y, Z z, W w) noexcept
	    : x(static_cast<T>(x))
	    , y(static_cast<T>(y))
	    , z(static_cast<T>(z))
	    , w(static_cast<T>(w))
	{
	}

	template<class U>
	constexpr vec4(const vec4<U>& v) noexcept
	    : x(static_cast<T>(v.x))
	    , y(static_cast<T>(v.y))
	    , z(static_cast<T>(v.z))
	    , w(static_cast<T>(v.w))
	{
	}

	template<class Z, class W>
	constexpr vec4(const vec2<T>& v, Z z, W w) noexcept
	    : x(v.x)
	    , y(v.y)
	    , z(static_cast<T>(z))
	    , w(static_cast<T>(w))
	{
	}

	constexpr vec4(const vec2<T>& v1, const vec2<T>& v2) noexcept
	    : x(v1.x)
	    , y(v1.y)
	    , z(v2.x)
	    , w(v2.y)
	{
	}

	template<class U, class Z, class W>
	constexpr vec4(const vec2<U>& v, Z z, W w) noexcept
	    : x(static_cast<T>(v.x))
	    , y(static_cast<T>(v.y))
	    , z(static_cast<T>(z))
	    , w(static_cast<T>(w))
	{
	}

	template<class U>
	constexpr vec4(const vec2<U>& v1, const vec2<U>& v2) noexcept
	    : x(static_cast<T>(v1.x))
	    , y(static_cast<T>(v1.y))
	    , z(static_cast<T>(v2.x))
	    , w(static_cast<T>(v2.y))
	{
	}

	template<class W>
	constexpr vec4(const vec3<T>& v, W w) noexcept
	    : x(v.x)
	    , y(v.y)
	    , z(v.z)
	    , w(static_cast<T>(w))
	{
	}

	template<class U, class W>
	constexpr vec4(const vec3<U>& v, W w) noexcept
	    : x(static_cast<T>(v.x))
	    , y(static_cast<T>(v.y))
	    , z(static_cast<T>(v.z))
	    , w(static_cast<T>(w))
	{
	}

	vec4& operator=(const vec4&) = default;

	vec4& operator=(vec4&&) = default;

	[[nodiscard]] constexpr bool operator==(const vec4& v) const noexcept
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	[[nodiscard]] constexpr bool operator!=(const vec4& v) const noexcept
	{
		return x != v.x && y != v.y && z != v.z && w != v.w;
	}

	[[nodiscard]] constexpr vec4 operator+(const vec4& v) const noexcept
	{
		return { x + v.x, y + v.y, z + v.z, w + v.w };
	}

	[[nodiscard]] constexpr vec4 operator-(const vec4& v) const noexcept
	{
		return { x - v.x, y - v.y, z - v.z, w - v.w };
	}

	[[nodiscard]] constexpr vec4 operator*(const vec4& v) const noexcept
	{
		return { x * v.x, y * v.y, z * v.z, w * v.w };
	}

	[[nodiscard]] constexpr vec4 operator/(const vec4& v) const noexcept
	{
		return { x / v.x, y / v.y, z / v.z, w / v.w };
	}

	vec4 operator+=(const vec4& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	vec4 operator-=(const vec4& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	vec4 operator*=(const vec4& v) noexcept
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	vec4 operator/=(const vec4& v) noexcept
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	vec4 operator=(const T& v) noexcept
	{
		x = v;
		y = v;
		z = v;
		w = v;
		return *this;
	}

	vec4 operator+=(const T& v) noexcept
	{
		x += v;
		y += v;
		z += v;
		w += v;
		return *this;
	}

	vec4 operator-=(const T& v) noexcept
	{
		x -= v;
		y -= v;
		z -= v;
		w -= v;
		return *this;
	}

	vec4 operator*=(const T& v) noexcept
	{
		x *= v;
		y *= v;
		z *= v;
		w *= v;
		return *this;
	}

	vec4 operator/=(const T& v) noexcept
	{
		x /= v;
		y /= v;
		z /= v;
		w /= v;
		return *this;
	}

#if defined(_ENABLE_D3D11) || defined(_ENABLE_D3D12)
	// --- xm_type helper ---

	vec4(const xm_type& v) noexcept
	    : x(v.x)
	    , y(v.y)
	    , z(v.z)
	    , w(v.w)
	{
	}

	operator xm_type() const noexcept
	{
		return xm_type(x, y, z, w);
	}

	// --- xmv_type helper ---

	vec4(const xmv_type& v) noexcept
	{
		if constexpr (std::is_same_v<xm_type, DirectX::XMINT4>) {
			DirectX::XMStoreSInt4(reinterpret_cast<xm_type*>(this), v);
		}
		else if constexpr (std::is_same_v<xm_type, DirectX::XMUINT4>) {
			DirectX::XMStoreUInt4(reinterpret_cast<xm_type*>(this), v);
		}
		else {
			DirectX::XMStoreFloat4(reinterpret_cast<xm_type*>(this), v);
		}
	}

	operator xmv_type() const noexcept
	{
		if constexpr (std::is_same_v<xm_type, DirectX::XMINT2>) {
			return DirectX::XMLoadSInt4(reinterpret_cast<const xm_type*>(this));
		}
		else if constexpr (std::is_same_v<xm_type, DirectX::XMUINT2>) {
			return DirectX::XMLoadUInt4(reinterpret_cast<const xm_type*>(this));
		}
		return DirectX::XMLoadFloat4(reinterpret_cast<const xm_type*>(this));
	}
#endif
};

} // namespace math
} // namespace dxlib

using int2 = dxlib::math::vec2<int32_t>;
using int3 = dxlib::math::vec3<int32_t>;
using int4 = dxlib::math::vec4<int32_t>;

using uint2 = dxlib::math::vec2<uint32_t>;
using uint3 = dxlib::math::vec3<uint32_t>;
using uint4 = dxlib::math::vec4<uint32_t>;

using float2 = dxlib::math::vec2<float>;
using float3 = dxlib::math::vec3<float>;
using float4 = dxlib::math::vec4<float>;
