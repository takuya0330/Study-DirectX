#include "d3d12_scene_triangle.h"

#include "dxlib/debug.h"
#include "dxlib/static_mesh.h"

namespace app {

d3d12_scene_triangle::d3d12_scene_triangle(ID3D12Device* d3d12_device, ID3D12GraphicsCommandList* d3d12_graphics_command_list)
    : m_d3d12_device(d3d12_device)
    , m_d3d12_graphics_command_list(d3d12_graphics_command_list)
    , m_d3d12_vertex_buffer()
    , m_d3d12_root_signature()
    , m_d3d12_pipeline_state()
{
	ASSERT(m_d3d12_device);
	ASSERT(m_d3d12_graphics_command_list);
}

bool d3d12_scene_triangle::initialize()
{
	dxlib::geometry::vertex_pc vertices[] = {
		{float3(+0.0f, +0.5f, 0.0f), float4(1.0f, 0.0f, 0.0f, 1.0f)},
		{float3(+0.5f, -0.5f, 0.0f), float4(0.0f, 1.0f, 0.0f, 1.0f)},
		{float3(-0.5f, -0.5f, 0.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)}
	};

	D3D12_HEAP_PROPERTIES heap_properties = {};
	{
		heap_properties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
		heap_properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask     = dxlib::d3d12::default_node_mask;
		heap_properties.VisibleNodeMask      = dxlib::d3d12::default_node_mask;
	}
	D3D12_RESOURCE_DESC resource_desc = {};
	{
		resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment          = 0;
		resource_desc.Width              = sizeof(vertices);
		resource_desc.Height             = 1;
		resource_desc.DepthOrArraySize   = 1;
		resource_desc.MipLevels          = 1;
		resource_desc.Format             = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count   = 1;
		resource_desc.SampleDesc.Quality = 0;
		resource_desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
	}
	auto hr = dxlib::d3d12::create_resource(
	    m_d3d12_device,
	    &heap_properties,
	    D3D12_HEAP_FLAG_NONE,
	    &resource_desc,
	    D3D12_RESOURCE_STATE_GENERIC_READ,
	    nullptr,
	    m_d3d12_vertex_buffer.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);
	{
		void* mapped = nullptr;
		hr           = m_d3d12_vertex_buffer->Map(0, nullptr, &mapped);
		ASSERT_RETURN(SUCCEEDED(hr), false);
		memcpy(mapped, vertices, sizeof(vertices));
		m_d3d12_vertex_buffer->Unmap(0, nullptr);
	}

	MSWRL::ComPtr<ID3DBlob> vertex_shader;
	hr = dxlib::d3d12::create_vertex_shader_from_hlsl(
	    L"../../asset/shader/static_mesh_pc_vs.hlsl",
	    "main",
	    "vs_5_0",
	    vertex_shader.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	MSWRL::ComPtr<ID3DBlob> pixel_shader;
	hr = dxlib::d3d12::create_pixel_shader_from_hlsl(
	    L"../../asset/shader/static_mesh_pc_ps.hlsl",
	    "main",
	    "ps_5_0",
	    pixel_shader.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
	{
		root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}
	hr = dxlib::d3d12::create_root_signature(
	    m_d3d12_device,
	    &root_signature_desc,
	    D3D_ROOT_SIGNATURE_VERSION_1_0,
	    dxlib::d3d12::default_node_mask,
	    m_d3d12_root_signature.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	D3D12_INPUT_ELEMENT_DESC input_element_desc[] = {
		{"POSITION", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{   "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	D3D12_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
	{
		render_target_blend_desc.BlendEnable           = false;
		render_target_blend_desc.LogicOpEnable         = false;
		render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gfx_pipeline_state_desc = {};
	{
		gfx_pipeline_state_desc.pRootSignature                        = m_d3d12_root_signature.Get();
		gfx_pipeline_state_desc.VS.pShaderBytecode                    = vertex_shader->GetBufferPointer();
		gfx_pipeline_state_desc.VS.BytecodeLength                     = vertex_shader->GetBufferSize();
		gfx_pipeline_state_desc.PS.pShaderBytecode                    = pixel_shader->GetBufferPointer();
		gfx_pipeline_state_desc.PS.BytecodeLength                     = pixel_shader->GetBufferSize();
		gfx_pipeline_state_desc.BlendState.AlphaToCoverageEnable      = false;
		gfx_pipeline_state_desc.BlendState.IndependentBlendEnable     = false;
		gfx_pipeline_state_desc.BlendState.RenderTarget[0]            = render_target_blend_desc;
		gfx_pipeline_state_desc.SampleMask                            = D3D12_DEFAULT_SAMPLE_MASK;
		gfx_pipeline_state_desc.RasterizerState.MultisampleEnable     = false;
		gfx_pipeline_state_desc.RasterizerState.CullMode              = D3D12_CULL_MODE_NONE;
		gfx_pipeline_state_desc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
		gfx_pipeline_state_desc.RasterizerState.DepthClipEnable       = true;
		gfx_pipeline_state_desc.RasterizerState.FrontCounterClockwise = false;
		gfx_pipeline_state_desc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
		gfx_pipeline_state_desc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		gfx_pipeline_state_desc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		gfx_pipeline_state_desc.RasterizerState.AntialiasedLineEnable = true;
		gfx_pipeline_state_desc.RasterizerState.ForcedSampleCount     = 0;
		gfx_pipeline_state_desc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		gfx_pipeline_state_desc.DepthStencilState.DepthEnable         = false;
		gfx_pipeline_state_desc.DepthStencilState.StencilEnable       = false;
		gfx_pipeline_state_desc.InputLayout.pInputElementDescs        = input_element_desc;
		gfx_pipeline_state_desc.InputLayout.NumElements               = _countof(input_element_desc);
		gfx_pipeline_state_desc.IBStripCutValue                       = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		gfx_pipeline_state_desc.PrimitiveTopologyType                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gfx_pipeline_state_desc.NumRenderTargets                      = 1;
		gfx_pipeline_state_desc.RTVFormats[0]                         = DXGI_FORMAT_R8G8B8A8_UNORM;
		gfx_pipeline_state_desc.SampleDesc.Count                      = 1;
		gfx_pipeline_state_desc.SampleDesc.Quality                    = 0;
	}
	hr = dxlib::d3d12::create_graphics_pipeline_state(
	    m_d3d12_device,
	    &gfx_pipeline_state_desc,
	    m_d3d12_pipeline_state.GetAddressOf());
	ASSERT_RETURN(SUCCEEDED(hr), false);

	return true;
}

void d3d12_scene_triangle::update()
{
	m_d3d12_graphics_command_list->SetPipelineState(m_d3d12_pipeline_state.Get());
	m_d3d12_graphics_command_list->SetGraphicsRootSignature(m_d3d12_root_signature.Get());
	m_d3d12_graphics_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	{
		vbv.BufferLocation = m_d3d12_vertex_buffer->GetGPUVirtualAddress();
		vbv.SizeInBytes    = sizeof(dxlib::geometry::vertex_pc) * 3;
		vbv.StrideInBytes  = sizeof(dxlib::geometry::vertex_pc);
	}
	m_d3d12_graphics_command_list->IASetVertexBuffers(0, 1, &vbv);
	m_d3d12_graphics_command_list->DrawInstanced(3, 1, 0, 0);
}

} // namespace app
