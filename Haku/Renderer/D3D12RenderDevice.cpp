#include "../macros.hpp"
#include "../Core/HakuLog.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "../Core/Exceptions.hpp"
namespace Haku
{
namespace Renderer
{
D3D12RenderDevice::D3D12RenderDevice()
{
	unsigned int FactoryFlags{};
#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> DebugDevice;
		HAKU_SOK_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugDevice)))
		FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		DebugDevice->EnableDebugLayer();
	}
#endif
	Microsoft::WRL::ComPtr<IDXGIAdapter> GraphicsAdapter; // Graphics adapter IUnknown
	Microsoft::WRL::ComPtr<IDXGIFactory6>
		DxgiFactory; // this might fail due to lack of support,might need to extend the interface
	HAKU_SOK_ASSERT(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&DxgiFactory)))
	HAKU_SOK_ASSERT(DxgiFactory->EnumAdapterByGpuPreference(
		0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&GraphicsAdapter)))

	DXGI_ADAPTER_DESC adpter_desc{};
	HAKU_SOK_ASSERT(GraphicsAdapter->GetDesc(&adpter_desc))

	HAKU_SOK_ASSERT(D3D12CreateDevice(
		GraphicsAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_Device))) // the support and request needs checking
}

void D3D12RenderDevice::ShutDown() noexcept
{
	m_Device.Reset();
}
} // namespace Renderer
} // namespace Haku
