#include <memory>

#include "app/d3d12/d3d12_scene_triangle.h"
#include "dxlib/d3d12_api.h"
#include "dxlib/debug.h"
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
		for (UINT32 i = 0; i < dxlib::dxgi::default_back_buffer_count; ++i) {
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

		if (d3d12_fence->GetCompletedValue() < d3d12_fence_values[back_buffer_index]) {
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

app::scene_base* get_next_scene(int index, ID3D12Device* d3d12_device, ID3D12GraphicsCommandList* d3d12_graphics_command_list)
{
	switch (index) {
	case 0:
		return new app::d3d12_scene_triangle(d3d12_device, d3d12_graphics_command_list);
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
	while (true) {
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {
			break;
		}

		context.begin_frame();
		{
			if (scene) {
				scene->update();
			}
		}
		context.end_frame();
	}

	// destroy window.
	dxlib::win32::destroy_window(wcex, hwnd);

	return 0;
}