#pragma once

#include <d3d12.h>

#include <vector>

#include "dxgi_api.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace dxlib {
namespace d3d12 {

//! \brief
inline constexpr UINT32 default_node_mask = 0;

//! \brief �f�o�b�O���C���[�̗L����
//!
//! \param[in] enable_gpu_based_validation
void enable_debug_layer(
    bool enable_gpu_based_validation);

//! \brief �f�o�C�X�̍쐬
//!
//! \param[in] dxgi_adapter
//! \param[out] d3d12_device
//!
//! \ret HRESULT
HRESULT create_device(
    IDXGIAdapter*  dxgi_adapter,
    ID3D12Device** d3d12_device);

//! \brief �R�}���h�L���[�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] d3d12_command_queue_desc
//! \param[out] d3d12_command_queue
//!
//! \ret HRESULT
HRESULT create_command_queue(
    ID3D12Device*                   d3d12_device,
    const D3D12_COMMAND_QUEUE_DESC* d3d12_command_queue_desc,
    ID3D12CommandQueue**            d3d12_command_queue);

//! \brief �R�}���h�A���P�[�^�[�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] d3d12_command_list_type
//! \param[out] d3d12_command_allocator
//!
//! \ret HRESULT
HRESULT create_command_allocator(
    ID3D12Device*                 d3d12_device,
    const D3D12_COMMAND_LIST_TYPE d3d12_command_list_type,
    ID3D12CommandAllocator**      d3d12_command_allocator);

//! \brief �R�}���h���X�g�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] node_mask
//! \param[in] d3d12_command_list_type
//! \param[in] d3d12_command_allocator
//! \param[out] d3d12_graphics_command_list
//!
//! \ret HRESULT
HRESULT create_graphics_command_list(
    ID3D12Device*                 d3d12_device,
    const UINT32                  node_mask,
    const D3D12_COMMAND_LIST_TYPE d3d12_command_list_type,
    ID3D12CommandAllocator*       d3d12_command_allocator,
    ID3D12GraphicsCommandList**   d3d12_graphics_command_list);

//! \brief �f�B�X�N���v�^�q�[�v�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] d3d12_descriptor_heap_desc
//! \param[out] d3d12_descriptor_heap
//! \param[out] d3d12_descriptor_size
//!
//! \ret HRESULT
HRESULT create_descriptor_heap(
    ID3D12Device*                     d3d12_device,
    const D3D12_DESCRIPTOR_HEAP_DESC* d3d12_descriptor_heap_desc,
    ID3D12DescriptorHeap**            d3d12_descriptor_heap,
    UINT32&                           d3d12_descriptor_size);

//! \brief �o�b�N�o�b�t�@�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] dxgi_swap_chain
//! \param[in] d3d12_back_buffer_heap
//! \param[out] d3d12_back_buffers
//!
//! \ret HRESULT
HRESULT create_back_buffer(
    ID3D12Device*                      d3d12_device,
    IDXGISwapChain*                    dxgi_swap_chain,
    const UINT32                       buffer_index,
    const D3D12_CPU_DESCRIPTOR_HANDLE& d3d12_back_buffer_handle,
    ID3D12Resource**                   d3d12_back_buffer);

//! \brief �t�F���X�̍쐬
//!
//! \param[in] d3d12_device
//! \param[out] d3d12_fence
//!
//! \ret HRESULT
HRESULT create_fence(
    ID3D12Device* d3d12_device,
    ID3D12Fence** d3d12_fence);

//! \brief ���[�g�V�O�l�`���̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] d3d12_root_signature_desc
//! \param[in] d3d_root_signature_version
//! \param[in] node_mask
//! \param[out] d3d12_root_signature
//!
//! \ret HRESULT
HRESULT create_root_signature(
    ID3D12Device*                    d3d12_device,
    const D3D12_ROOT_SIGNATURE_DESC* d3d12_root_signature_desc,
    const D3D_ROOT_SIGNATURE_VERSION d3d_root_signature_version,
    const UINT                       node_mask,
    ID3D12RootSignature**            d3d12_root_signature);

//! \brief �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] d3d12_graphics_pipeline_state_desc
//! \param[out] d3d12_pipeline_state
//!
//! \ret HRESULT
HRESULT create_graphics_pipeline_state(
    ID3D12Device*                             d3d12_device,
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC* d3d12_graphics_pipeline_state_desc,
    ID3D12PipelineState**                     d3d12_pipeline_state);

//! \brief ���_�V�F�[�_�[�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] filename
//! \param[in] entry_point
//! \param[in] shader_model
//! \param[out] d3d12_vertex_shader
//! \param[out] d3d12_input_element_descs
//!
//! \ret HRESULT
HRESULT create_vertex_shader_from_hlsl(
    ID3D12Device*              d3d12_device,
    const wchar_t*             filename,
    const char*                entry_point,
    const char*                shader_model,
    ID3DBlob**                 d3d12_vertex_shader);

//! \brief �s�N�Z���V�F�[�_�[�̍쐬
//!
//! \param[in] d3d12_device
//! \param[in] filename
//! \param[in] entry_point
//! \param[in] shader_model
//! \param[out] d3d12_pixel_shader
//!
//! \ret HRESULT
HRESULT create_pixel_shader_from_hlsl(
    ID3D12Device*  d3d12_device,
    const wchar_t* filename,
    const char*    entry_point,
    const char*    shader_model,
    ID3DBlob**     d3d12_pixel_shader);

} // namespace d3d12
} // namespace dxlib
