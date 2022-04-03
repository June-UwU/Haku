#pragma once
#include "wil/wrl.h"
#include "../../macros.hpp"
#include "D3D12RenderDevice.hpp"
#include <d3d12.h>
namespace Haku
{
namespace Renderer
{
class CommandQueue
{
public:
	CommandQueue(
		RenderDevice&			  device,
		D3D12_COMMAND_LIST_TYPE	  type,
		D3D12_COMMAND_QUEUE_FLAGS flags	   = D3D12_COMMAND_QUEUE_FLAG_NONE,
		INT						  priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL);
	~CommandQueue();
	void					Reset();
	void					Shutdown() noexcept;
	ID3D12CommandQueue*		Get() noexcept { return m_CommandQueue; };
	ID3D12CommandAllocator* GetAllocator() noexcept { return m_CommandAllocator[m_CurrentIndex]; }

private:
	uint32_t				m_CurrentIndex;
	ID3D12CommandQueue*		m_CommandQueue;
	ID3D12CommandAllocator* m_CommandAllocator[FrameCount];
};

class D3D12CommandQueue
{
public:
	D3D12CommandQueue(D3D12RenderDevice& Device);
	// this will change with the development of PSO class development
	void					   Close();
	void					   Execute();
	void					   Synchronize();
	void					   ShutDown() noexcept;
	void					   ResetCommandAllocator();
	void					   CloseFenceHandle() noexcept;
	void					   Reset(ID3D12PipelineState* PipelineState);
	ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList.Get(); }
	ID3D12CommandQueue*		   GetCommandQueue() { return m_CommandQueue.Get(); }
	void					   ResetCommandList(ID3D12PipelineState* PipelineState);
	ID3D12CommandAllocator*	   GetCommandAllocator() { return m_CommandAllocator[m_FenceValue % FrameCount].Get(); }

private:
	// Synchronization objects.
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	HANDLE								m_FenceEvent;
	uint64_t							m_FenceValue;
	// Render and GPU commands helpers for command list
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>		  m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	  m_CommandAllocator[FrameCount];
};
} // namespace Renderer
} // namespace Haku
