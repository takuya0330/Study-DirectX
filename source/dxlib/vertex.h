#pragma once

#include "vector.h"

namespace dxlib {
namespace geometry {

struct vertex_p
{
	float3 position;
};

struct vertex_pc : public vertex_p
{
	float4 color;
};

struct vertex_pu : public vertex_p
{
	float2 uv;
};

struct vertex_puc : public vertex_pu
{
	float4 color;
};

struct vertex_pn : public vertex_p
{
	float3 normal;
};

struct vertex_pnu : public vertex_pn
{
	float2 uv;
};

} // namespace geometry
} // namespace dxlib
