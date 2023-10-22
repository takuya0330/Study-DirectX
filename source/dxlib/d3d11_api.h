#pragma once

#include <d3d11.h>

#include "dxgi_api.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace dxlib {
namespace d3d11 {

HRESULT create_device_and_device_context(
    IDXGIAdapter*         dxgi_adapter,
    ID3D11Device**        d3d11_device,
    ID3D11DeviceContext** d3d11_device_context);

HRESULT create_back_buffer(
    ID3D11Device*            d3d11_device,
    IDXGISwapChain*          dxgi_swapchain,
    ID3D11RenderTargetView** d3d11_back_buffer_view);

HRESULT create_buffer(
    ID3D11Device*                 d3d11_device,
    const D3D11_BUFFER_DESC*      d3d11_buffer_desc,
    const D3D11_SUBRESOURCE_DATA* d3d11_subresource_data,
    ID3D11Buffer**                d3d11_buffer);

HRESULT create_texture2d(
    ID3D11Device*                 d3d11_device,
    const D3D11_TEXTURE2D_DESC*   d3d11_texture2d_desc,
    const D3D11_SUBRESOURCE_DATA* d3d11_subresource_data,
    ID3D11Texture2D**             d3d11_texture2d);

HRESULT create_render_target_view(
    ID3D11Device*               d3d11_device,
    const D3D11_TEXTURE2D_DESC* d3d11_texture2d_desc,
    ID3D11RenderTargetView**    d3d11_render_target_view);

HRESULT create_depth_stencil_view(
    ID3D11Device*               d3d11_device,
    const D3D11_TEXTURE2D_DESC* d3d11_texture2d_desc,
    ID3D11DepthStencilView**    d3d11_depth_stencil_view);

HRESULT create_shader_resource_view(
    ID3D11Device*               d3d11_device,
    const D3D11_TEXTURE2D_DESC* d3d11_texture2d_desc,
    ID3D11ShaderResourceView**  d3d11_shader_resource_view);

HRESULT create_blend_state(
    ID3D11Device*           d3d11_device,
    const D3D11_BLEND_DESC* d3d11_blend_desc,
    ID3D11BlendState**      d3d11_blend_state);

HRESULT create_depth_stencil_state(
    ID3D11Device*                   d3d11_device,
    const D3D11_DEPTH_STENCIL_DESC* d3d11_depth_stencil_desc,
    ID3D11DepthStencilState**       d3d11_depth_stencil_state);

HRESULT create_rasterizer_state(
    ID3D11Device*                d3d11_device,
    const D3D11_RASTERIZER_DESC* d3d11_rasterizer_desc,
    ID3D11RasterizerState**      d3d11_rasterizer_state);

HRESULT create_sampler_state(
    ID3D11Device*             d3d11_device,
    const D3D11_SAMPLER_DESC* d3d11_sampler_desc,
    ID3D11SamplerState**      d3d11_sampler_state);

HRESULT create_vertex_shader_from_hlsl(
    ID3D11Device*        d3d11_device,
    const wchar_t*       filename,
    const char*          entry_point,
    const char*          shader_model,
    ID3D11VertexShader** d3d11_vertex_shader,
    ID3D11InputLayout**  d3d11_input_layout);

HRESULT create_hull_shader_from_hlsl(
    ID3D11Device*      d3d11_device,
    const wchar_t*     filename,
    const char*        entry_point,
    const char*        shader_model,
    ID3D11HullShader** d3d11_hull_shader);

HRESULT create_domain_shader_from_hlsl(
    ID3D11Device*        d3d11_device,
    const wchar_t*       filename,
    const char*          entry_point,
    const char*          shader_model,
    ID3D11DomainShader** d3d11_domain_shader);

HRESULT create_geometry_shader_from_hlsl(
    ID3D11Device*          d3d11_device,
    const wchar_t*         filename,
    const char*            entry_point,
    const char*            shader_model,
    ID3D11GeometryShader** d3d11_geometry_shader);

HRESULT create_pixel_shader_from_hlsl(
    ID3D11Device*       d3d11_device,
    const wchar_t*      filename,
    const char*         entry_point,
    const char*         shader_model,
    ID3D11PixelShader** d3d11_pixel_shader);

HRESULT create_compute_shader_from_hlsl(
    ID3D11Device*         d3d11_device,
    const wchar_t*        filename,
    const char*           entry_point,
    const char*           shader_model,
    ID3D11ComputeShader** d3d11_compute_shader);

} // namespace d3d11
} // namespace dxlib