#pragma once

#include "app/scene_base.h"
#include "dxlib/d3d11_api.h"

namespace app {

class d3d11_scene_triangle : public scene_base
{
public:
	d3d11_scene_triangle(ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_device_context);

	~d3d11_scene_triangle() = default;

	bool initialize() override;

	void update() override;
private:
	ID3D11Device*                     m_d3d11_device;
	ID3D11DeviceContext*              m_d3d11_device_context;
	MSWRL::ComPtr<ID3D11Buffer>       m_d3d11_vertex_buffer;
	MSWRL::ComPtr<ID3D11VertexShader> m_d3d11_vertex_shader;
	MSWRL::ComPtr<ID3D11InputLayout>  m_d3d11_input_layout;
	MSWRL::ComPtr<ID3D11PixelShader>  m_d3d11_pixel_shader;
};

} // namespace app