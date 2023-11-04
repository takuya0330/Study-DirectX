#include "d3d11_scene_triangle.h"

#include "dxlib/debug.h"
#include "dxlib/static_mesh.h"

namespace app {

d3d11_scene_triangle::d3d11_scene_triangle(ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_device_context)
    : m_d3d11_device(d3d11_device)
    , m_d3d11_device_context(d3d11_device_context)
    , m_d3d11_vertex_buffer()
    , m_d3d11_vertex_shader()
    , m_d3d11_input_layout()
    , m_d3d11_pixel_shader()
{
	ASSERT(m_d3d11_device);
	ASSERT(m_d3d11_device_context);
}

bool d3d11_scene_triangle::initialize()
{
	dxlib::geometry::vertex_pc vertices[] = {
		{float3(+0.0f, +0.5f, 0.0f), float4(1.0f, 0.0f, 0.0f, 1.0f)},
		{float3(+0.5f, -0.5f, 0.0f), float4(0.0f, 1.0f, 0.0f, 1.0f)},
		{float3(-0.5f, -0.5f, 0.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)}
	};

	D3D11_BUFFER_DESC buffer_desc = {};
	{
		buffer_desc.ByteWidth           = sizeof(vertices);
		buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags      = 0;
		buffer_desc.MiscFlags           = 0;
		buffer_desc.StructureByteStride = 0;
	}
	D3D11_SUBRESOURCE_DATA subresource = {};
	{
		subresource.pSysMem          = vertices;
		subresource.SysMemPitch      = 0;
		subresource.SysMemSlicePitch = 0;
	}
	auto hr = dxlib::d3d11::create_buffer(
	    m_d3d11_device,
	    &buffer_desc,
	    &subresource,
	    m_d3d11_vertex_buffer.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	hr = dxlib::d3d11::create_vertex_shader_from_hlsl(
	    m_d3d11_device,
	    L"../../asset/shader/static_mesh_pc_vs.hlsl",
	    "main",
	    "vs_5_0",
	    m_d3d11_vertex_shader.GetAddressOf(),
	    m_d3d11_input_layout.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	hr = dxlib::d3d11::create_pixel_shader_from_hlsl(
	    m_d3d11_device,
	    L"../../asset/shader/static_mesh_pc_ps.hlsl",
	    "main",
	    "ps_5_0",
	    m_d3d11_pixel_shader.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	return true;
}

void d3d11_scene_triangle::update()
{
	const uint32_t stride = sizeof(dxlib::geometry::vertex_pc);
	const uint32_t offset = 0;
	m_d3d11_device_context->IASetInputLayout(m_d3d11_input_layout.Get());
	m_d3d11_device_context->IASetVertexBuffers(0, 1, m_d3d11_vertex_buffer.GetAddressOf(), &stride, &offset);
	m_d3d11_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_d3d11_device_context->VSSetShader(m_d3d11_vertex_shader.Get(), nullptr, 0);
	m_d3d11_device_context->PSSetShader(m_d3d11_pixel_shader.Get(), nullptr, 0);
	m_d3d11_device_context->Draw(3, 0);
}

} // namespace app