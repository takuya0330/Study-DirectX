#pragma once

#include "vector.h"

namespace dxlib {
namespace scene {

struct directional_light
{
	float3 direction;
	float  intensity;
	float4 color;
};

struct point_light
{
	float3 position;
	float  intensity;
	float3 color;
	float  range;
};

struct spot_light
{
	float3 position;
	float  intensity;
	float3 direction;
	float  angle;
	float3 color;
	float  range;
};

} // namespace scene
} // namespace dxlib
