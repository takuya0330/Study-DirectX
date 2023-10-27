#include "d3d12_api.h"

#include <d3dcompiler.h>

#include "debug.h"
#include "file.h"

namespace {

HRESULT compile_shader(ID3DBlob** blob, const wchar_t* filename, const char* entry_point, const char* shader_model)
{
	HRESULT hr = S_OK;

#if defined(_DEBUG)
	constexpr UINT32 compile_flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	constexpr UINT32 compile_flag = 0;
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
namespace d3d12 {

void enable_debug_layer(
    bool enable_gpu_based_validation)
{
	MSWRL::ComPtr<ID3D12Debug> debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())))) {
		debug->EnableDebugLayer();

		if (enable_gpu_based_validation) {
			MSWRL::ComPtr<ID3D12Debug3> debug3;
			debug.As(&debug3);
			debug3->SetEnableGPUBasedValidation(true);
		}
	}
}

HRESULT create_device(
    IDXGIAdapter*  dxgi_adaptor,
    ID3D12Device** d3d12_device)
{
	HRESULT hr = S_OK;

	hr = D3D12CreateDevice(dxgi_adaptor, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(d3d12_device));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_command_queue(
    ID3D12Device*                   d3d12_device,
    const D3D12_COMMAND_QUEUE_DESC* d3d12_command_queue_desc,
    ID3D12CommandQueue**            d3d12_command_queue)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateCommandQueue(d3d12_command_queue_desc, IID_PPV_ARGS(d3d12_command_queue));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_command_allocator(
    ID3D12Device*            d3d12_device,
    D3D12_COMMAND_LIST_TYPE  d3d12_command_list_type,
    ID3D12CommandAllocator** d3d12_command_allocator)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateCommandAllocator(d3d12_command_list_type, IID_PPV_ARGS(d3d12_command_allocator));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_graphics_command_list(
    ID3D12Device*                 d3d12_device,
    const UINT32                  node_mask,
    const D3D12_COMMAND_LIST_TYPE d3d12_command_list_type,
    ID3D12CommandAllocator*       d3d12_command_allocator,
    ID3D12GraphicsCommandList**   d3d12_graphics_command_list)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateCommandList(node_mask, d3d12_command_list_type, d3d12_command_allocator, nullptr, IID_PPV_ARGS(d3d12_graphics_command_list));
	RETURN_IF_FAILED(hr, hr);

	hr = (*d3d12_graphics_command_list)->Close();
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_descriptor_heap(
    ID3D12Device*                     d3d12_device,
    const D3D12_DESCRIPTOR_HEAP_DESC* d3d12_descriptor_heap_desc,
    ID3D12DescriptorHeap**            d3d12_descriptor_heap,
    UINT32&                           d3d12_descriptor_size)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateDescriptorHeap(d3d12_descriptor_heap_desc, IID_PPV_ARGS(d3d12_descriptor_heap));
	RETURN_IF_FAILED(hr, hr);
	d3d12_descriptor_size = d3d12_device->GetDescriptorHandleIncrementSize(d3d12_descriptor_heap_desc->Type);

	return hr;
}

HRESULT create_back_buffer(
    ID3D12Device*                      d3d12_device,
    IDXGISwapChain*                    dxgi_swap_chain,
    const UINT32                       buffer_index,
    const D3D12_CPU_DESCRIPTOR_HANDLE& d3d12_back_buffer_handle,
    ID3D12Resource**                   d3d12_back_buffer)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);
	ASSERT_RETURN(dxgi_swap_chain, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3D12Resource> back_buffer;
	hr = dxgi_swap_chain->GetBuffer(buffer_index, IID_PPV_ARGS(back_buffer.GetAddressOf()));
	RETURN_IF_FAILED(hr, hr);

	d3d12_device->CreateRenderTargetView(back_buffer.Get(), nullptr, d3d12_back_buffer_handle);

	*d3d12_back_buffer = back_buffer.Detach();

	return hr;
}

HRESULT create_fence(
    ID3D12Device* d3d12_device,
    ID3D12Fence** d3d12_fence)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3d12_fence));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_resource(
    ID3D12Device*                d3d12_device,
    const D3D12_HEAP_PROPERTIES* d3d12_heap_properties,
    const D3D12_HEAP_FLAGS       d3d12_heap_flags,
    const D3D12_RESOURCE_DESC*   d3d12_resource_desc,
    const D3D12_RESOURCE_STATES  d3d12_resource_states,
    const D3D12_CLEAR_VALUE*     d3d12_clear_color,
    ID3D12Resource**             d3d12_resource)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateCommittedResource(
	    d3d12_heap_properties,
	    d3d12_heap_flags,
	    d3d12_resource_desc,
	    d3d12_resource_states,
	    d3d12_clear_color,
	    IID_PPV_ARGS(d3d12_resource));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_root_signature(
    ID3D12Device*                    d3d12_device,
    const D3D12_ROOT_SIGNATURE_DESC* d3d12_root_signature_desc,
    const D3D_ROOT_SIGNATURE_VERSION d3d_root_signature_version,
    const UINT                       node_mask,
    ID3D12RootSignature**            d3d12_root_signature)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> root_signature_blob;
	MSWRL::ComPtr<ID3DBlob> error_blob;
	hr = D3D12SerializeRootSignature(d3d12_root_signature_desc, d3d_root_signature_version, root_signature_blob.GetAddressOf(), error_blob.GetAddressOf());
	if (FAILED(hr)) {
		if (error_blob) {
			_LOG_ERROR_MSG((char*)error_blob->GetBufferPointer());
		}
		return hr;
	}

	hr = d3d12_device->CreateRootSignature(node_mask, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(), IID_PPV_ARGS(d3d12_root_signature));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

HRESULT create_graphics_pipeline_state(
    ID3D12Device*                             d3d12_device,
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC* d3d12_graphics_pipeline_state_desc,
    ID3D12PipelineState**                     d3d12_pipeline_state)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	hr = d3d12_device->CreateGraphicsPipelineState(d3d12_graphics_pipeline_state_desc, IID_PPV_ARGS(d3d12_pipeline_state));
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

#if 1
HRESULT create_vertex_shader_from_hlsl(
    const wchar_t* filename,
    const char*    entry_point,
    const char*    shader_model,
    ID3DBlob**     d3d12_vertex_shader)
{
	HRESULT hr = compile_shader(d3d12_vertex_shader, filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}
#else
HRESULT create_vertex_shader_from_hlsl(
    ID3D12Device*              d3d12_device,
    const wchar_t*             filename,
    const char*                entry_point,
    const char*                shader_model,
    ID3DBlob**                 d3d12_vertex_shader,
    D3D12_INPUT_ELEMENT_DESCS& d3d12_input_element_descs)
{
	ASSERT_RETURN(d3d12_device, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<ID3DBlob> blob;
	hr = compile_shader(blob.GetAddressOf(), filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	MSWRL::ComPtr<ID3D12ShaderReflection> reflection;
	hr = D3DReflect(
	    blob->GetBufferPointer(),
	    blob->GetBufferSize(),
	    IID_ID3D11ShaderReflection,
	    reinterpret_cast<void**>(reflection.GetAddressOf()));
	RETURN_IF_FAILED(hr, hr);

	D3D12_SHADER_DESC shader_desc = {};
	hr                            = reflection->GetDesc(&shader_desc);
	RETURN_IF_FAILED(hr, hr);

	D3D12_INPUT_ELEMENT_DESCS input_element_descs;
	for (uint32_t i = 0; i < shader_desc.InputParameters; ++i) {
		D3D12_SIGNATURE_PARAMETER_DESC param_desc = {};
		hr                                        = reflection->GetInputParameterDesc(i, &param_desc);
		RETURN_IF_FAILED(hr, hr);

		// TODO: D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA ‚É‘Î‰ž
		D3D12_INPUT_ELEMENT_DESC desc = {
			desc.SemanticName         = param_desc.SemanticName,
			desc.SemanticIndex        = param_desc.SemanticIndex,
			desc.Format               = conv_format(param_desc.ComponentType, param_desc.Mask),
			desc.InputSlot            = 0,
			desc.AlignedByteOffset    = D3D12_APPEND_ALIGNED_ELEMENT,
			desc.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			desc.InstanceDataStepRate = 0,
		};
		input_element_descs.push_back(desc);
	}

	*d3d12_vertex_shader      = blob.Detach();
	d3d12_input_element_descs = std::move(input_element_descs);

	return hr;
}
#endif

HRESULT create_pixel_shader_from_hlsl(
    const wchar_t* filename,
    const char*    entry_point,
    const char*    shader_model,
    ID3DBlob**     d3d12_pixel_shader)
{
	HRESULT hr = compile_shader(d3d12_pixel_shader, filename, entry_point, shader_model);
	RETURN_IF_FAILED(hr, hr);

	return hr;
}

} // namespace d3d12
} // namespace dxlib