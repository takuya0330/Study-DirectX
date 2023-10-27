#include <memory>

#include "app/scene_base.h"
#include "dxlib/d3d12_api.h"
#include "dxlib/debug.h"
#include "dxlib/static_mesh.h"
#include "dxlib/window.h"

namespace {

struct d3d12_context
{
	MSWRL::ComPtr<IDXGIFactory6>             dxgi_factory;
	MSWRL::ComPtr<IDXGIAdapter4>             dxgi_adapter;
	MSWRL::ComPtr<ID3D12Device>              d3d12_device;
	MSWRL::ComPtr<ID3D12CommandQueue>        d3d12_command_queue;
	MSWRL::ComPtr<IDXGISwapChain4>           dxgi_swap_chain;
	MSWRL::ComPtr<ID3D12DescriptorHeap>      d3d12_back_buffer_heap;
	UINT32                                   d3d12_back_buffer_heap_size = 0;
	MSWRL::ComPtr<ID3D12Resource>            d3d12_back_buffers[dxlib::dxgi::default_back_buffer_count];
	MSWRL::ComPtr<ID3D12CommandAllocator>    d3d12_command_allocators[dxlib::dxgi::default_back_buffer_count];
	MSWRL::ComPtr<ID3D12GraphicsCommandList> d3d12_graphics_command_list;
	MSWRL::ComPtr<ID3D12Fence>               d3d12_fences[dxlib::dxgi::default_back_buffer_count];
	UINT64                                   d3d12_fence_values[dxlib::dxgi::default_back_buffer_count] = { 0 };
	D3D12_VIEWPORT                           d3d12_viewport                                             = {};
	D3D12_RECT                               d3d12_scissor                                              = {};

	bool initialize(HWND hwnd)
	{
		HRESULT hr = S_OK;

		dxlib::d3d12::enable_debug_layer(false);

		hr = dxlib::dxgi::create_factory(dxgi_factory.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		hr = dxlib::dxgi::search_adapter(dxgi_factory.Get(), dxgi_adapter.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		hr = dxlib::d3d12::create_device(dxgi_adapter.Get(), d3d12_device.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		D3D12_COMMAND_QUEUE_DESC d3d12_command_queue_desc = {};
		{
			d3d12_command_queue_desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
			d3d12_command_queue_desc.Priority = 0;
			d3d12_command_queue_desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
			d3d12_command_queue_desc.NodeMask = dxlib::d3d12::default_node_mask;
		}
		hr = dxlib::d3d12::create_command_queue(
		    d3d12_device.Get(),
		    &d3d12_command_queue_desc,
		    d3d12_command_queue.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		DXGI_SWAP_CHAIN_DESC dxgi_swap_chain_desc = {};
		{
			dxgi_swap_chain_desc.BufferDesc.Width                   = dxlib::win32::default_window_width;
			dxgi_swap_chain_desc.BufferDesc.Height                  = dxlib::win32::default_window_height;
			dxgi_swap_chain_desc.BufferDesc.RefreshRate.Numerator   = 60;
			dxgi_swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
			dxgi_swap_chain_desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
			dxgi_swap_chain_desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			dxgi_swap_chain_desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
			dxgi_swap_chain_desc.SampleDesc.Count                   = 1;
			dxgi_swap_chain_desc.SampleDesc.Quality                 = 0;
			dxgi_swap_chain_desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			dxgi_swap_chain_desc.BufferCount                        = dxlib::dxgi::default_back_buffer_count;
			dxgi_swap_chain_desc.OutputWindow                       = hwnd;
			dxgi_swap_chain_desc.Windowed                           = true;
			dxgi_swap_chain_desc.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			dxgi_swap_chain_desc.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		}
		hr = dxlib::dxgi::create_swap_chain(
		    d3d12_command_queue.Get(),
		    dxgi_factory.Get(),
		    &dxgi_swap_chain_desc,
		    dxgi_swap_chain.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		D3D12_DESCRIPTOR_HEAP_DESC d3d12_back_buffer_heap_desc = {};
		{
			d3d12_back_buffer_heap_desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			d3d12_back_buffer_heap_desc.NumDescriptors = dxlib::dxgi::default_back_buffer_count;
			d3d12_back_buffer_heap_desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			d3d12_back_buffer_heap_desc.NodeMask       = dxlib::d3d12::default_node_mask;
		}
		hr = dxlib::d3d12::create_descriptor_heap(
		    d3d12_device.Get(),
		    &d3d12_back_buffer_heap_desc,
		    d3d12_back_buffer_heap.GetAddressOf(),
		    d3d12_back_buffer_heap_size);
		ASSERT_RETURN(SUCCEEDED(hr), false);

		D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_handle = d3d12_back_buffer_heap->GetCPUDescriptorHandleForHeapStart();
		for (UINT32 i = 0; i < dxlib::dxgi::default_back_buffer_count; ++i)
		{
			hr = dxlib::d3d12::create_back_buffer(
			    d3d12_device.Get(),
			    dxgi_swap_chain.Get(),
			    i,
			    back_buffer_handle,
			    d3d12_back_buffers[i].GetAddressOf());
			ASSERT_RETURN(SUCCEEDED(hr), false);
			back_buffer_handle.ptr += d3d12_back_buffer_heap_size;

			hr = dxlib::d3d12::create_command_allocator(
			    d3d12_device.Get(),
			    D3D12_COMMAND_LIST_TYPE_DIRECT,
			    d3d12_command_allocators[i].GetAddressOf());
			ASSERT_RETURN(SUCCEEDED(hr), false);

			hr = dxlib::d3d12::create_fence(
			    d3d12_device.Get(),
			    d3d12_fences[i].GetAddressOf());
			ASSERT_RETURN(SUCCEEDED(hr), false);
		}

		hr = dxlib::d3d12::create_graphics_command_list(
		    d3d12_device.Get(),
		    dxlib::d3d12::default_node_mask,
		    D3D12_COMMAND_LIST_TYPE_DIRECT,
		    d3d12_command_allocators[0].Get(),
		    d3d12_graphics_command_list.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		{
			d3d12_viewport.TopLeftX = 0;
			d3d12_viewport.TopLeftY = 0;
			d3d12_viewport.Width    = dxlib::win32::default_window_width;
			d3d12_viewport.Height   = dxlib::win32::default_window_height;
			d3d12_viewport.MinDepth = 0.0f;
			d3d12_viewport.MaxDepth = 1.0f;
		}
		{
			d3d12_scissor.left   = 0;
			d3d12_scissor.top    = 0;
			d3d12_scissor.right  = dxlib::win32::default_window_width;
			d3d12_scissor.bottom = dxlib::win32::default_window_height;
		}

		return true;
	}

	void begin_frame()
	{
		auto back_buffer_index = dxgi_swap_chain->GetCurrentBackBufferIndex();

		auto d3d12_command_allocator = d3d12_command_allocators[back_buffer_index].Get();
		d3d12_command_allocator->Reset();
		d3d12_graphics_command_list->Reset(d3d12_command_allocator, nullptr);

		auto                   d3d12_back_buffer = d3d12_back_buffers[back_buffer_index].Get();
		D3D12_RESOURCE_BARRIER barrier           = {};
		{
			barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = d3d12_back_buffer;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		d3d12_graphics_command_list->ResourceBarrier(1, &barrier);

		constexpr float default_clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		auto            d3d12_back_buffer_view = d3d12_back_buffer_heap->GetCPUDescriptorHandleForHeapStart();
		d3d12_back_buffer_view.ptr += static_cast<size_t>(back_buffer_index * d3d12_back_buffer_heap_size);
		d3d12_graphics_command_list->OMSetRenderTargets(1, &d3d12_back_buffer_view, false, nullptr);
		d3d12_graphics_command_list->ClearRenderTargetView(d3d12_back_buffer_view, default_clear_color, 0, nullptr);
		d3d12_graphics_command_list->RSSetViewports(1, &d3d12_viewport);
		d3d12_graphics_command_list->RSSetScissorRects(1, &d3d12_scissor);
	}

	void end_frame()
	{
		auto                   back_buffer_index = dxgi_swap_chain->GetCurrentBackBufferIndex();
		auto                   d3d12_back_buffer = d3d12_back_buffers[back_buffer_index].Get();
		D3D12_RESOURCE_BARRIER barrier           = {};
		{
			barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource   = d3d12_back_buffer;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		}
		d3d12_graphics_command_list->ResourceBarrier(1, &barrier);

		d3d12_graphics_command_list->Close();
		ID3D12CommandList* d3d12_command_lists[] = {
			d3d12_graphics_command_list.Get()
		};
		d3d12_command_queue->ExecuteCommandLists(1, d3d12_command_lists);

		dxgi_swap_chain->Present(1, 0);

		auto       d3d12_fence       = d3d12_fences[back_buffer_index].Get();
		const auto d3d12_fence_value = ++d3d12_fence_values[back_buffer_index];
		d3d12_command_queue->Signal(d3d12_fence, d3d12_fence_value);

		back_buffer_index = dxgi_swap_chain->GetCurrentBackBufferIndex();

		if (d3d12_fence->GetCompletedValue() < d3d12_fence_values[back_buffer_index])
		{
			auto fence_event = CreateEvent(nullptr, false, false, nullptr);
			d3d12_fence->SetEventOnCompletion(d3d12_fence_values[back_buffer_index], fence_event);
#pragma warning(push)
#pragma warning(disable : 6387)
			WaitForSingleObjectEx(fence_event, INFINITE, false);
#pragma warning(pop)
		}

		d3d12_fence_values[back_buffer_index] = d3d12_fence_value + 1;
	}
};

class d3d12_scene_triangle : public app::scene_base
{
public:
	d3d12_scene_triangle(ID3D12Device* d3d12_device, ID3D12GraphicsCommandList* d3d12_graphics_command_list)
	    : m_d3d12_device(d3d12_device)
	    , m_d3d12_graphics_command_list(d3d12_graphics_command_list)
	    , m_d3d12_vertex_buffer()
	    , m_d3d12_root_signature()
	    , m_d3d12_pipeline_state()
	{
		ASSERT(m_d3d12_device);
		ASSERT(m_d3d12_graphics_command_list);
	}

	~d3d12_scene_triangle() = default;

	bool initialize() override
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

	void update() override
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

private:
	ID3D12Device*                      m_d3d12_device;
	ID3D12GraphicsCommandList*         m_d3d12_graphics_command_list;
	MSWRL::ComPtr<ID3D12Resource>      m_d3d12_vertex_buffer;
	MSWRL::ComPtr<ID3D12RootSignature> m_d3d12_root_signature;
	MSWRL::ComPtr<ID3D12PipelineState> m_d3d12_pipeline_state;
};

app::scene_base* get_next_scene(int index, ID3D12Device* d3d12_device, ID3D12GraphicsCommandList* d3d12_graphics_command_list)
{
	switch (index) {
	case 0:
		return new d3d12_scene_triangle(d3d12_device, d3d12_graphics_command_list);
	default:
		break;
	}
	return nullptr;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
	case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			break;
		}
	case WM_PAINT:
		{
			HDC         hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

} // namespace

int main(int argc, char* argv[])
{
	ENABLE_CRT_DEBUG_FLAG;

	// create application window.
	auto wcex = dxlib::win32::register_window_class_ex(
	    L"DirectX",
	    WndProc);
	auto hwnd = dxlib::win32::create_window(
	    L"DirectX",
	    L"D3D12 Sandbox",
	    WS_OVERLAPPEDWINDOW,
	    dxlib::win32::default_window_width,
	    dxlib::win32::default_window_height);
	ASSERT_RETURN(hwnd, -1);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// create d3d12.
	d3d12_context context;
	ASSERT_RETURN(context.initialize(hwnd), -1);

	// create scene.
	constexpr const char* scene_type[] = {
		"Triangle",
		"Cube",
	};
	int scene_type_index      = 0;
	int scene_type_prev_index = scene_type_index;

	std::unique_ptr<app::scene_base> scene(get_next_scene(
	    scene_type_index,
	    context.d3d12_device.Get(),
	    context.d3d12_graphics_command_list.Get()));
	ASSERT_RETURN(scene->initialize(), -1);

	// main loop.
	while (true)
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}

		context.begin_frame();
		{
			if (scene)
			{
				scene->update();
			}
		}
		context.end_frame();
	}

	// destroy window.
	dxlib::win32::destroy_window(wcex, hwnd);

	return 0;
}