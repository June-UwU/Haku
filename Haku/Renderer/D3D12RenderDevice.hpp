#pragma once
#include "wil/wrl.h"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include <dxgi1_6.h>
#include "../Platform/Windows/MainWindow.hpp"
//TODO
//shift the this to a consumable class
namespace Haku
{
namespace Renderer
{
class D3D12CommandQueue;
class D3D12RenderDevice
{
public:
	D3D12RenderDevice();
	void		  ShutDown() noexcept;
	ID3D12Device* get() { return m_Device.Get(); }
	
private:
	Microsoft::WRL::ComPtr<ID3D12Device>	m_Device;
};
} // namespace Renderer
} // namespace Haku
