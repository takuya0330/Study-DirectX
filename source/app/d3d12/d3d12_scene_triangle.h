#pragma once

#include "app/scene_base.h"
#include "dxlib/d3d12_api.h"

namespace app {

class d3d12_scene_triangle : public scene_base
{
public:
	d3d12_scene_triangle(ID3D12Device* d3d12_device, ID3D12GraphicsCommandList* d3d12_graphics_command_list);

	~d3d12_scene_triangle() = default;

	bool initialize() override;

	void update() override;

private:
	ID3D12Device*                      m_d3d12_device;
	ID3D12GraphicsCommandList*         m_d3d12_graphics_command_list;
	MSWRL::ComPtr<ID3D12Resource>      m_d3d12_vertex_buffer;
	MSWRL::ComPtr<ID3D12RootSignature> m_d3d12_root_signature;
	MSWRL::ComPtr<ID3D12PipelineState> m_d3d12_pipeline_state;
};

} // namespace app