#pragma once

#include "macros.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"

namespace Haku
{
namespace Renderer
{
class Fence
{
public:
	Fence(D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);
	~Fence() noexcept;

	ID3D12Fence* Get() noexcept;
	void		 Close() noexcept;
	HANDLE&		 GetEvent() noexcept;

private:
	void ShutDown() noexcept;

private:
	ID3D12Fence* m_Fence;
	HANDLE		 m_FenceEvent;
};
} // namespace Renderer
} // namespace Haku
