#pragma once

#include "directx/d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../Windows/MainWindow.hpp"
#include "wil/wrl.h"
namespace Haku
{
namespace Renderer
{
class D3D12RenderDevice
{
public:
	D3D12RenderDevice();
	void		  Render();
	void		  FrameIndexReset();
	ID3D12Device* get() { return m_Device.Get(); }
	void		  BackBuffer(ID3D12GraphicsCommandList* list);
	void		  RenderTarget(ID3D12GraphicsCommandList* list);
	void		  init(Haku::Windows* window, ID3D12CommandQueue* CommandQueue);

private:
	Microsoft::WRL::ComPtr<ID3D12Device>	m_Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
	uint32_t								m_FrameIndex = 0;
	// Render targets
	// rtv needs a descriptor..? this method is still not understood clearly
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	static const uint32_t						 FrameCount			 = 2;
	uint32_t									 m_RtvDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource>		 m_RenderTargets[FrameCount];
};
} // namespace Renderer
} // namespace Haku
