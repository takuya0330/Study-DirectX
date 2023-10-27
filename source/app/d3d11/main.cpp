#include <memory>

#include "app/scene_base.h"
#include "dxlib/d3d11_api.h"
#include "dxlib/debug.h"
#include "dxlib/static_mesh.h"
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

class d3d11_scene_triangle : public app::scene_base
{
public:
	d3d11_scene_triangle(ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_device_context)
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

	~d3d11_scene_triangle() = default;

	bool initialize() override
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

	void update() override
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

private:
	ID3D11Device*                     m_d3d11_device;
	ID3D11DeviceContext*              m_d3d11_device_context;
	MSWRL::ComPtr<ID3D11Buffer>       m_d3d11_vertex_buffer;
	MSWRL::ComPtr<ID3D11VertexShader> m_d3d11_vertex_shader;
	MSWRL::ComPtr<ID3D11InputLayout>  m_d3d11_input_layout;
	MSWRL::ComPtr<ID3D11PixelShader>  m_d3d11_pixel_shader;
};

app::scene_base* get_next_scene(int index, ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_device_context)
{
	switch (index) {
	case 0:
		return new d3d11_scene_triangle(d3d11_device, d3d11_device_context);
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