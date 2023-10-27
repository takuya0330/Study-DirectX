#include "d3d11_api.h"

#include <d3dcommon.h>
#include <d3dcompiler.h>

#include <vector>

#include "debug.h"
#include "file.h"

namespace {

HRESULT compile_shader(ID3DBlob** blob, const wchar_t* filename, const char* entry_point, const char* shader_model)
{
	HRESULT hr = S_OK;

#if defined(_DEBUG)
	constexpr UINT32 compile_flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	constexpr UINT32 compile_flag       = 0;
#endif

	MSWRL::ComPtr<ID3DBlob> error;
	hr = D3DCompileFromFile(
	    filename,
	    nullptr,
	    D3D_COMPILE_STANDARD_FILE_INCLUDE,
	    entry_point,
	    shader_model,
	    compile_flag,
	    0,
	    blob,
	    error.GetAddressOf());
	if (FAILED(hr) && error) {
		_LOG_ERROR_MSG((char*)error->GetBufferPointer());
	}
	return hr;
}

DXGI_FORMAT conv_format(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask)
{
	if (mask == 0x01) {
		switch (type) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
	else if (mask == 0x03) {
		switch (type) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
	else if (mask == 0x07) {
		switch (type) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
	else if (mask == 0x0F) {
		switch (type) {
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case D3D_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
	return DXGI_FORMAT_UNKNOWN;
}

} // namespace

namespace dxlib {
namespace d3d11 {

HRESULT create_device_and_device_context(
    IDXGIAdapter*         dxgi_adapter,
    ID3D11Device**        d3d11_device,
    ID3D11DeviceContext** d3d11_device_context)
{
	ASSERT_RETURN(dxgi_adapter, E_UNEXPECTED);

	HRESULT hr = S_OK;

#if defined(_DEBUG)
	UINT32 create_device_flag = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT32           create_device_flag = 0;
#endif
	constexpr D3D_DRIVER_TYPE d3d_driver_type[] = {
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	constexpr D3D_FEATURE_LEVEL d3d_feature_level[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL feature_level;
	for (D3D_DRIVER_TYPE driver_type : d3d_driver_type) {
		hr = D3D11CreateDevice(
		    dxgi_adapter,
		    driver_type,
		    nullptr,
		    create_device_flag,
		    d3d_feature_level,
		    _countof(d3d_feature_level),
		    D3D11_SDK_VERSION,
		    d3d11_device,
		    &feature_level,
		    d3d11_device_context);
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_back_buffer(
    ID3D11Device*            d3d11_device,
    IDXGISwapChain*          dxgi_swapchain,
    ID3D11RenderTargetView** d3d11_render_target_view)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(dxgi_swapchain, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3D11Texture2D> back_buffer;
	hr = dxgi_swapchain->GetBuffer(0, IID_PPV_ARGS(back_buffer.ReleaseAndGetAddressOf()));
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreateRenderTargetView(back_buffer.Get(), nullptr, d3d11_render_target_view);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_buffer(
    ID3D11Device*                 d3d11_device,
    const D3D11_BUFFER_DESC*      d3d11_buffer_desc,
    const D3D11_SUBRESOURCE_DATA* d3d11_subresource_data,
    ID3D11Buffer**                d3d11_buffer)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_buffer_desc, E_UNEXPECTED);

	auto hr = d3d11_device->CreateBuffer(d3d11_buffer_desc, d3d11_subresource_data, d3d11_buffer);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_texture2d(
    ID3D11Device*                 d3d11_device,
    const D3D11_TEXTURE2D_DESC*   d3d11_texture2d_desc,
    const D3D11_SUBRESOURCE_DATA* d3d11_subresource_data,
    ID3D11Texture2D**             d3d11_texture2d)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_texture2d_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d11_device->CreateTexture2D(d3d11_texture2d_desc, d3d11_subresource_data, d3d11_texture2d);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_render_target_view(
    ID3D11Device*               d3d11_device,
    const D3D11_TEXTURE2D_DESC* d3d11_texture2d_desc,
    ID3D11RenderTargetView**    d3d11_render_target_view)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_texture2d_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3D11Texture2D> d3d11_texture2d;
	hr = create_texture2d(d3d11_device, d3d11_texture2d_desc, nullptr, d3d11_texture2d.GetAddressOf());
	RETURN_IF_FAILED(hr, hr);

	D3D11_RENDER_TARGET_VIEW_DESC d3d11_render_target_view_desc = {};
	{
		d3d11_render_target_view_desc.Format             = d3d11_texture2d_desc->Format;
		d3d11_render_target_view_desc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
		d3d11_render_target_view_desc.Texture2D.MipSlice = 0;
	}
	hr = d3d11_device->CreateRenderTargetView(
	    d3d11_texture2d.Get(),
	    &d3d11_render_target_view_desc,
	    d3d11_render_target_view);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_depth_stencil_view(
    ID3D11Device*               d3d11_device,
    const D3D11_TEXTURE2D_DESC* d3d11_texture2d_desc,
    ID3D11DepthStencilView**    d3d11_depth_stencil_view)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_texture2d_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3D11Texture2D> d3d11_texture2d;
	hr = create_texture2d(d3d11_device, d3d11_texture2d_desc, nullptr, d3d11_texture2d.GetAddressOf());
	RETURN_IF_FAILED(hr, hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_depth_stencil_view_desc = {};
	{
		d3d11_depth_stencil_view_desc.Format             = d3d11_texture2d_desc->Format;
		d3d11_depth_stencil_view_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3d11_depth_stencil_view_desc.Flags              = 0;
		d3d11_depth_stencil_view_desc.Texture2D.MipSlice = 0;
	}
	hr = d3d11_device->CreateDepthStencilView(
	    d3d11_texture2d.Get(),
	    &d3d11_depth_stencil_view_desc,
	    d3d11_depth_stencil_view);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_shader_resource_view(
    ID3D11Device*               d3d11_device,
    const D3D11_TEXTURE2D_DESC* d3d11_texture2d_desc,
    ID3D11ShaderResourceView**  d3d11_shader_resource_view)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_texture2d_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3D11Texture2D> d3d11_texture2d;
	hr = create_texture2d(d3d11_device, d3d11_texture2d_desc, nullptr, d3d11_texture2d.GetAddressOf());
	RETURN_IF_FAILED(hr, hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_desc = {};
	{
		d3d11_shader_resource_view_desc.Format              = d3d11_texture2d_desc->Format;
		d3d11_shader_resource_view_desc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
		d3d11_shader_resource_view_desc.Texture2D.MipLevels = 1;
	}
	hr = d3d11_device->CreateShaderResourceView(
	    d3d11_texture2d.Get(),
	    &d3d11_shader_resource_view_desc,
	    d3d11_shader_resource_view);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_blend_state(
    ID3D11Device*           d3d11_device,
    const D3D11_BLEND_DESC* d3d11_blend_desc,
    ID3D11BlendState**      d3d11_blend_state)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_blend_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d11_device->CreateBlendState(d3d11_blend_desc, d3d11_blend_state);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_depth_stencil_state(
    ID3D11Device*                   d3d11_device,
    const D3D11_DEPTH_STENCIL_DESC* d3d11_depth_stencil_desc,
    ID3D11DepthStencilState**       d3d11_depth_stencil_state)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_depth_stencil_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d11_device->CreateDepthStencilState(d3d11_depth_stencil_desc, d3d11_depth_stencil_state);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_rasterizer_state(
    ID3D11Device*                d3d11_device,
    const D3D11_RASTERIZER_DESC* d3d11_rasterizer_desc,
    ID3D11RasterizerState**      d3d11_rasterizer_state)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_rasterizer_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d11_device->CreateRasterizerState(d3d11_rasterizer_desc, d3d11_rasterizer_state);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_sampler_state(
    ID3D11Device*             d3d11_device,
    const D3D11_SAMPLER_DESC* d3d11_sampler_desc,
    ID3D11SamplerState**      d3d11_sampler_state)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);
	ASSERT_RETURN(d3d11_sampler_desc, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d11_device->CreateSamplerState(d3d11_sampler_desc, d3d11_sampler_state);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_vertex_shader_from_hlsl(
    ID3D11Device*        d3d11_device,
    const wchar_t*       filename,
    const char*          entry_point,
    const char*          shader_model,
    ID3D11VertexShader** d3d11_vertex_shader,
    ID3D11InputLayout**  d3d11_input_layout)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreateVertexShader(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    nullptr,
	    d3d11_vertex_shader);
	RETURN_IF_FAILED(hr, hr);

	MSWRL::ComPtr<ID3D11ShaderReflection> reflection;
	hr = D3DReflect(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    IID_ID3D11ShaderReflection,
	    reinterpret_cast<void**>(reflection.GetAddressOf()));
	RETURN_IF_FAILED(hr, hr);

	D3D11_SHADER_DESC shader_desc = {};
	hr                            = reflection->GetDesc(&shader_desc);
	RETURN_IF_FAILED(hr, hr);

	std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_desc;
	for (uint32_t i = 0; i < shader_desc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC param_desc = {};
		hr                                        = reflection->GetInputParameterDesc(i, &param_desc);
		RETURN_IF_FAILED(hr, hr);

		// TODO: D3D11_INPUT_PER_INSTANCE_DATA ‚É‘Î‰ž
		D3D11_INPUT_ELEMENT_DESC desc = {};
		{
			desc.SemanticName         = param_desc.SemanticName;
			desc.SemanticIndex        = param_desc.SemanticIndex;
			desc.Format               = conv_format(param_desc.ComponentType, param_desc.Mask);
			desc.InputSlot            = 0;
			desc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
		}
		input_layout_desc.push_back(desc);
	}

	hr = d3d11_device->CreateInputLayout(
	    input_layout_desc.data(),
	    static_cast<UINT>(input_layout_desc.size()),
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    d3d11_input_layout);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_hull_shader_from_hlsl(
    ID3D11Device*      d3d11_device,
    const wchar_t*     filename,
    const char*        entry_point,
    const char*        shader_model,
    ID3D11HullShader** d3d11_hull_shader)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreateHullShader(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    nullptr,
	    d3d11_hull_shader);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_domain_shader_from_hlsl(
    ID3D11Device*        d3d11_device,
    const wchar_t*       filename,
    const char*          entry_point,
    const char*          shader_model,
    ID3D11DomainShader** d3d11_domain_shader)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreateDomainShader(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    nullptr,
	    d3d11_domain_shader);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_geometry_shader_from_hlsl(
    ID3D11Device*          d3d11_device,
    const wchar_t*         filename,
    const char*            entry_point,
    const char*            shader_model,
    ID3D11GeometryShader** d3d11_geometry_shader)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreateGeometryShader(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    nullptr,
	    d3d11_geometry_shader);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_pixel_shader_from_hlsl(
    ID3D11Device*       d3d11_device,
    const wchar_t*      filename,
    const char*         entry_point,
    const char*         shader_model,
    ID3D11PixelShader** d3d11_pixel_shader)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreatePixelShader(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    nullptr,
	    d3d11_pixel_shader);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_compute_shader_from_hlsl(
    ID3D11Device*         d3d11_device,
    const wchar_t*        filename,
    const char*           entry_point,
    const char*           shader_model,
    ID3D11ComputeShader** d3d11_compute_shader)
{
	ASSERT_RETURN(d3d11_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	hr = d3d11_device->CreateComputeShader(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    nullptr,
	    d3d11_compute_shader);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

} // namespace d3d11
} // namespace dxlib