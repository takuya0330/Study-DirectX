#pragma once

#include "vector.h"

namespace dxlib {
namespace scene {

struct camera
{
	float3 position;
	float3 focus;
	float3 up;
	float  near_z;
	float  far_z;
	float  fov_y;
};

} // namespace scene
} // namespace dxlib
