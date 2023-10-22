#pragma once

#include <dxgi1_6.h>
#include <wrl/client.h>

#pragma comment(lib, "dxgi.lib")

namespace MSWRL = Microsoft::WRL;

namespace dxlib {
namespace dxgi {

//! \brief バックバッファの最大数
inline constexpr UINT32 default_back_buffer_count = 2;

//! \brief ファクトリーの作成
//!
//! \param[out] dxgi_factory
HRESULT create_factory(
    IDXGIFactory6** dxgi_factory);

//! \brief ビデオカードの検索
//!
//! \param[in] dxgi_factory
//! \param[out] dxgi_adapter
HRESULT search_adapter(
    IDXGIFactory6*  dxgi_factory,
    IDXGIAdapter4** dxgi_adapter);

//! \brief スワップチェインの作成
//!
//! \param[in] device
//! \param[in] dxgi_factory
//! \param[in] dxgi_swap_chain_desc
//! \param[out] dxgi_swap_chain
HRESULT create_swap_chain(
    IUnknown*             device,
    IDXGIFactory6*        dxgi_factory,
    DXGI_SWAP_CHAIN_DESC* dxgi_swap_chain_desc,
    IDXGISwapChain4**     dxgi_swap_chain);

} // namespace dxgi
} // namespace dxlib