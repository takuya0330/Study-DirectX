#include "dxgi_api.h"

#include "debug.h"

namespace dxlib {
namespace dxgi {

HRESULT create_factory(
    IDXGIFactory6** dxgi_factory)
{
#if defined(_DEBUG)
	constexpr UINT32 dxgi_create_factory_flag = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr UINT32 dxgi_create_factory_flag = 0;
#endif
	HRESULT hr = S_OK;

	MSWRL::ComPtr<IDXGIFactory6> dxgi_factory6;
	MSWRL::ComPtr<IDXGIFactory2> dxgi_factory2;
	hr = CreateDXGIFactory2(dxgi_create_factory_flag, IID_PPV_ARGS(dxgi_factory2.GetAddressOf()));
	RETURN_IF_FAILED(hr, hr);

	hr = dxgi_factory2.As(&dxgi_factory6);
	RETURN_IF_FAILED(hr, hr);

	*dxgi_factory = dxgi_factory6.Detach();

	return hr;
}

HRESULT search_adapter(
    IDXGIFactory6*  dxgi_factory,
    IDXGIAdapter4** dxgi_adapter)
{
	ASSERT_RETURN(dxgi_factory, E_UNEXPECTED);

	HRESULT hr = S_OK;

	bool                         enable_wrap_adaptor = true;
	DXGI_ADAPTER_DESC3           adaptor_desc        = {};
	MSWRL::ComPtr<IDXGIAdapter4> dxgi_adapter4;

	// 高性能なものから優先で列挙
	for (UINT32 i = 0; dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(dxgi_adapter4.GetAddressOf())) != DXGI_ERROR_NOT_FOUND; ++i) {
		hr = dxgi_adapter4->GetDesc3(&adaptor_desc);
		RETURN_IF_FAILED(hr, hr);

		// ハードウェアアダプターでサポートされているものを探す
		if (!(adaptor_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
			enable_wrap_adaptor = false;
			break;
		}
	}

	// GPUが無い場合CPUでエミュレートする
	if (enable_wrap_adaptor) {
		hr = dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(dxgi_adapter4.GetAddressOf()));
		RETURN_IF_FAILED(hr, hr);
	}

	*dxgi_adapter = dxgi_adapter4.Detach();

	return hr;
}

HRESULT create_swap_chain(
    IUnknown*             device,
    IDXGIFactory6*        dxgi_factory,
    DXGI_SWAP_CHAIN_DESC* dxgi_swap_chain_desc,
    IDXGISwapChain4**     dxgi_swap_chain)
{
	ASSERT_RETURN(device, E_UNEXPECTED);
	ASSERT_RETURN(dxgi_factory, E_UNEXPECTED);

	HRESULT hr = S_OK;

	MSWRL::ComPtr<IDXGISwapChain> swap_chain;
	hr = dxgi_factory->CreateSwapChain(device, dxgi_swap_chain_desc, swap_chain.GetAddressOf());
	RETURN_IF_FAILED(hr, hr);

	MSWRL::ComPtr<IDXGISwapChain4> swap_chain4;
	hr = swap_chain.As(&swap_chain4);
	RETURN_IF_FAILED(hr, hr);

	*dxgi_swap_chain = swap_chain4.Detach();

	return hr;
}

} // namespace dxgi
} // namespace dxlib