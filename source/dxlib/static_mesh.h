#pragma once

#include <vector>

#include "vertex.h"

namespace dxlib {
namespace geometry {

struct static_mesh_quad_p
{
	constexpr static_mesh_quad_p()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, 0.0f) });
		vertices.push_back({ float3(+0.5f, +0.5f, 0.0f) });
		vertices.push_back({ float3(-0.5f, -0.5f, 0.0f) });
		vertices.push_back({ float3(+0.5f, -0.5f, 0.0f) });

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_p) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_p> vertices;
	std::vector<uint16_t> indices;
	uint32_t              vertex_size;
	uint32_t              index_size;
};

struct static_mesh_quad_pc
{
	constexpr static_mesh_quad_pc()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, 0.0f), float4(1.0f) });
		vertices.push_back({ float3(+0.5f, +0.5f, 0.0f), float4(1.0f) });
		vertices.push_back({ float3(-0.5f, -0.5f, 0.0f), float4(1.0f) });
		vertices.push_back({ float3(+0.5f, -0.5f, 0.0f), float4(1.0f) });

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_pc) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_pc> vertices;
	std::vector<uint16_t>  indices;
	uint32_t               vertex_size;
	uint32_t               index_size;
};

struct static_mesh_quad_pu
{
	constexpr static_mesh_quad_pu()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, 0.0f), float2(0.0f, 0.0f) });
		vertices.push_back({ float3(+0.5f, +0.5f, 0.0f), float2(1.0f, 0.0f) });
		vertices.push_back({ float3(-0.5f, -0.5f, 0.0f), float2(0.0f, 1.0f) });
		vertices.push_back({ float3(+0.5f, -0.5f, 0.0f), float2(1.0f, 1.0f) });

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_pu) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_pu> vertices;
	std::vector<uint16_t>  indices;
	uint32_t               vertex_size;
	uint32_t               index_size;
};

struct static_mesh_quad_puc
{
	constexpr static_mesh_quad_puc()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, 0.0f), float2(0.0f, 0.0f), float4(1.0f) });
		vertices.push_back({ float3(+0.5f, +0.5f, 0.0f), float2(1.0f, 0.0f), float4(1.0f) });
		vertices.push_back({ float3(-0.5f, -0.5f, 0.0f), float2(0.0f, 1.0f), float4(1.0f) });
		vertices.push_back({ float3(+0.5f, -0.5f, 0.0f), float2(1.0f, 1.0f), float4(1.0f) });

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_puc) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_puc> vertices;
	std::vector<uint16_t>   indices;
	uint32_t                vertex_size;
	uint32_t                index_size;
};

struct static_mesh_quad_pn
{
	constexpr static_mesh_quad_pn()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f) });
		vertices.push_back({ float3(+0.5f, +0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f) });
		vertices.push_back({ float3(-0.5f, -0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f) });
		vertices.push_back({ float3(+0.5f, -0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f) });

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_pn) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_pn> vertices;
	std::vector<uint16_t>  indices;
	uint32_t               vertex_size;
	uint32_t               index_size;
};

struct static_mesh_quad_pnu
{
	constexpr static_mesh_quad_pnu()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });
		vertices.push_back({ float3(+0.5f, +0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f), float2(1.0f, 0.0f) });
		vertices.push_back({ float3(-0.5f, -0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 1.0f) });
		vertices.push_back({ float3(+0.5f, -0.5f, 0.0f), float3(0.0f, 0.0f, -1.0f), float2(1.0f, 1.0f) });

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_pnu) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_pnu> vertices;
	std::vector<uint16_t>   indices;
	uint32_t                vertex_size;
	uint32_t                index_size;
};

struct static_mesh_cube_pn
{
	constexpr static_mesh_cube_pn()
	{
		vertices.push_back({ float3(-0.5f, +0.5f, +0.5f), float3(0, 1, 0) });
		vertices.push_back({ float3(+0.5f, +0.5f, +0.5f), float3(0, 1, 0) });
		vertices.push_back({ float3(-0.5f, +0.5f, -0.5f), float3(0, 1, 0) });
		vertices.push_back({ float3(+0.5f, +0.5f, -0.5f), float3(0, 1, 0) });
		indices.push_back(0 * 4 + 0);
		indices.push_back(0 * 4 + 1);
		indices.push_back(0 * 4 + 2);
		indices.push_back(0 * 4 + 1);
		indices.push_back(0 * 4 + 3);
		indices.push_back(0 * 4 + 2);

		vertices.push_back({ float3(-0.5f, -0.5f, +0.5f), float3(0, -1, 0) });
		vertices.push_back({ float3(+0.5f, -0.5f, +0.5f), float3(0, -1, 0) });
		vertices.push_back({ float3(-0.5f, -0.5f, -0.5f), float3(0, -1, 0) });
		vertices.push_back({ float3(+0.5f, -0.5f, -0.5f), float3(0, -1, 0) });
		indices.push_back(1 * 4 + 0);
		indices.push_back(1 * 4 + 2);
		indices.push_back(1 * 4 + 1);
		indices.push_back(1 * 4 + 1);
		indices.push_back(1 * 4 + 2);
		indices.push_back(1 * 4 + 3);

		vertices.push_back({ float3(-0.5f, +0.5f, -0.5f), float3(0, 0, -1) });
		vertices.push_back({ float3(+0.5f, +0.5f, -0.5f), float3(0, 0, -1) });
		vertices.push_back({ float3(-0.5f, -0.5f, -0.5f), float3(0, 0, -1) });
		vertices.push_back({ float3(+0.5f, -0.5f, -0.5f), float3(0, 0, -1) });
		indices.push_back(2 * 4 + 0);
		indices.push_back(2 * 4 + 1);
		indices.push_back(2 * 4 + 2);
		indices.push_back(2 * 4 + 1);
		indices.push_back(2 * 4 + 3);
		indices.push_back(2 * 4 + 2);

		vertices.push_back({ float3(-0.5f, +0.5f, +0.5f), float3(0, 0, 1) });
		vertices.push_back({ float3(+0.5f, +0.5f, +0.5f), float3(0, 0, 1) });
		vertices.push_back({ float3(-0.5f, -0.5f, +0.5f), float3(0, 0, 1) });
		vertices.push_back({ float3(+0.5f, -0.5f, +0.5f), float3(0, 0, 1) });
		indices.push_back(3 * 4 + 0);
		indices.push_back(3 * 4 + 2);
		indices.push_back(3 * 4 + 1);
		indices.push_back(3 * 4 + 1);
		indices.push_back(3 * 4 + 2);
		indices.push_back(3 * 4 + 3);

		vertices.push_back({ float3(+0.5f, +0.5f, -0.5f), float3(1, 0, 0) });
		vertices.push_back({ float3(+0.5f, +0.5f, +0.5f), float3(1, 0, 0) });
		vertices.push_back({ float3(+0.5f, -0.5f, -0.5f), float3(1, 0, 0) });
		vertices.push_back({ float3(+0.5f, -0.5f, +0.5f), float3(1, 0, 0) });
		indices.push_back(4 * 4 + 0);
		indices.push_back(4 * 4 + 1);
		indices.push_back(4 * 4 + 2);
		indices.push_back(4 * 4 + 1);
		indices.push_back(4 * 4 + 3);
		indices.push_back(4 * 4 + 2);

		vertices.push_back({ float3(-0.5f, +0.5f, -0.5f), float3(-1, 0, 0) });
		vertices.push_back({ float3(-0.5f, +0.5f, +0.5f), float3(-1, 0, 0) });
		vertices.push_back({ float3(-0.5f, -0.5f, -0.5f), float3(-1, 0, 0) });
		vertices.push_back({ float3(-0.5f, -0.5f, +0.5f), float3(-1, 0, 0) });
		indices.push_back(5 * 4 + 0);
		indices.push_back(5 * 4 + 2);
		indices.push_back(5 * 4 + 1);
		indices.push_back(5 * 4 + 1);
		indices.push_back(5 * 4 + 2);
		indices.push_back(5 * 4 + 3);

		vertex_size = static_cast<uint32_t>(sizeof(vertex_pn) * vertices.size());
		index_size  = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	}

	std::vector<vertex_pn> vertices;
	std::vector<uint16_t>  indices;
	uint32_t               vertex_size;
	uint32_t               index_size;
};

} // namespace geometry
} // namespace dxlib
