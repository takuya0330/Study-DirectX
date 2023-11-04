#include <memory>

#include "app/d3d11/d3d11_scene_triangle.h"
#include "dxlib/d3d11_api.h"
#include "dxlib/debug.h"
#include "dxlib/window.h"

namespace {

struct d3d11_context
{
	MSWRL::ComPtr<IDXGIFactory6>          dxgi_factory;
	MSWRL::ComPtr<IDXGIAdapter4>          dxgi_adapter;
	MSWRL::ComPtr<ID3D11Device>           d3d11_device;
	MSWRL::ComPtr<ID3D11DeviceContext>    d3d11_immediate_context;
	MSWRL::ComPtr<IDXGISwapChain4>        dxgi_swap_chain;
	MSWRL::ComPtr<ID3D11RenderTargetView> d3d11_back_buffer_view;
	D3D11_VIEWPORT                        d3d11_viewport = {};

	bool initialize(HWND hwnd)
	{
		HRESULT hr = S_OK;

		hr = dxlib::dxgi::create_factory(dxgi_factory.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		hr = dxlib::dxgi::search_adapter(
		    dxgi_factory.Get(),
		    dxgi_adapter.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		hr = dxlib::d3d11::create_device_and_device_context(
		    dxgi_adapter.Get(),
		    d3d11_device.GetAddressOf(),
		    d3d11_immediate_context.GetAddressOf());
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
		    d3d11_device.Get(),
		    dxgi_factory.Get(),
		    &dxgi_swap_chain_desc,
		    dxgi_swap_chain.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		hr = dxlib::d3d11::create_back_buffer(
		    d3d11_device.Get(),
		    dxgi_swap_chain.Get(),
		    d3d11_back_buffer_view.GetAddressOf());
		ASSERT_RETURN(SUCCEEDED(hr), false);

		{
			d3d11_viewport.TopLeftX = 0;
			d3d11_viewport.TopLeftY = 0;
			d3d11_viewport.Width    = dxlib::win32::default_window_width;
			d3d11_viewport.Height   = dxlib::win32::default_window_height;
			d3d11_viewport.MinDepth = 0.0f;
			d3d11_viewport.MaxDepth = 1.0f;
		}

		return true;
	}

	void begin_frame()
	{
		constexpr float default_clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		d3d11_immediate_context->OMSetRenderTargets(1, d3d11_back_buffer_view.GetAddressOf(), nullptr);
		d3d11_immediate_context->ClearRenderTargetView(d3d11_back_buffer_view.Get(), default_clear_color);
		d3d11_immediate_context->RSSetViewports(1, &d3d11_viewport);
	}

	void end_frame()
	{
		dxgi_swap_chain->Present(1, 0);
	}
};

app::scene_base* get_next_scene(int index, ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_device_context)
{
	switch (index) {
	case 0:
		return new app::d3d11_scene_triangle(d3d11_device, d3d11_device_context);
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
	    L"D3D11 Sandbox",
	    WS_OVERLAPPEDWINDOW,
	    dxlib::win32::default_window_width,
	    dxlib::win32::default_window_height);
	ASSERT_RETURN(hwnd, -1);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// create d3d11.
	d3d11_context context;
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
	    context.d3d11_device.Get(),
	    context.d3d11_immediate_context.Get()));
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