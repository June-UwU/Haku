#include "D3D12Fence.hpp"
#include "Exceptions.hpp"
#include "D3D12Renderer.hpp"

namespace Haku
{
namespace Renderer
{
Fence::Fence(D3D12_FENCE_FLAGS flags)
{
	auto initial_value = RenderEngine::GetFenceValue();
	auto Device		   = RenderEngine::GetDeviceD3D();
	HAKU_SOK_ASSERT(Device->CreateFence(initial_value, flags, IID_PPV_ARGS(&m_Fence)))
	initial_value++;
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_FenceEvent)
	{
		HAKU_SOK_ASSERT(HRESULT_FROM_WIN32(GetLastError()))
	}
	HAKU_DXNAME(m_Fence, L"Synchroning Fence");
}
Fence::~Fence() noexcept
{
	Close();
	ShutDown();
}
HANDLE& Fence::GetEvent() noexcept
{
	return m_FenceEvent;
}
ID3D12Fence* Fence::Get() noexcept
{
	return m_Fence;
}
//void Fence::Synchronize()
//{
//	// Get the device and synchronizing value
//	auto		   fencevalue  = RenderEngine::GetFenceValue();
//	//auto		   DirectQueue = RenderEngine::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
//	auto		   Swapchain   = RenderEngine::GetSwapChain(); //this is taken to update the backbuffer index (m_FrameIndex)
//	const uint64_t fence_value = fencevalue;
//	//HAKU_SOK_ASSERT(DirectQueue->Get()->Signal(m_Fence, fence_value))
//	// updating fence value
//	RenderEngine::FenceValueAdd();
//	if (m_Fence->GetCompletedValue() < fence_value)
//	{
//		HAKU_SOK_ASSERT(m_Fence->SetEventOnCompletion(fence_value, m_FenceEvent))
//		WaitForSingleObject(m_FenceEvent, INFINITE);
//	}
//	Swapchain->SetBackBufferIndex();
//}
void Fence::ShutDown() noexcept
{
	m_Fence->Release();
}
void Fence::Close() noexcept
{
	CloseHandle(m_FenceEvent);
}
} // namespace Renderer
} // namespace Haku
