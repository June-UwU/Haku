#pragma once
#include "wil/wrl.h"
#include "D3D12RenderDevice.hpp"
#include "../../Renderer/RenderCommandQueue.hpp"
#include <d3d12.h>
namespace Haku
{
namespace Renderer
{
class D3D12CommandQueue : public RenderCommandQueue
{
public:
	D3D12CommandQueue(D3D12RenderDevice& Device);
	// this will change with the development of PSO class development
	void Close() override;
	void Execute() override;
	void Synchronize() override;
	// void					   PopulateCommandList();
	void					   ResetCommandAllocator();
	void					   Reset(ID3D12PipelineState* PipelineState);
	ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList.Get(); }
	ID3D12CommandQueue*		   GetCommandQueue() { return m_CommandQueue.Get(); }
	void					   ResetCommandList(ID3D12PipelineState* PipelineState);
	void					   CommandListCreate(D3D12RenderDevice& Device, ID3D12PipelineState* PipelineState);

private:
	// Synchronization objects.
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	HANDLE								m_FenceEvent;
	uint64_t							m_FenceValue;

	// Render and GPU commands helpers for command list
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>		  m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	  m_CommandAllocator;
};
} // namespace Renderer
} // namespace Haku
