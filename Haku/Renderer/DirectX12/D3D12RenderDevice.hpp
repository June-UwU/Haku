#pragma once
#include "wil/wrl.h"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include <dxgi1_6.h>
#include "../../Platform/Windows/MainWindow.hpp"
// TODO
// shift the this to a consumable class
namespace Haku
{
namespace Renderer
{
class RenderDevice
{
public:
	RenderDevice();
	~RenderDevice();
	void		  ShutDown() noexcept;
	ID3D12Device* Get() noexcept { return m_Device; };

private:
	ID3D12Device* m_Device = nullptr;
};

class D3D12RenderDevice
{
public:
	D3D12RenderDevice();
	~D3D12RenderDevice();
	void		  ShutDown() noexcept;
	ID3D12Device* get() { return m_Device; }
	ID3D12Device* Get() { return m_Device; }

private:
	ID3D12Device* m_Device = nullptr;
};
} // namespace Renderer
} // namespace Haku
