#pragma once
#include "../../macros.hpp"
#include "../../hakupch.hpp"
#include "directx/d3d12.h"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12PipelineState.hpp"
#include "D3D12RootSignature.hpp"
#include "../../Core/Exceptions.hpp"

// TODO : functions to port over
// m_Signature->SetSignature(*m_Command);
// m_DescriptorHeap->SetDescriptorHeaps(*m_Command);
//
// m_SwapChain->SetAndClearRenderTarget(*m_Command, *m_DescriptorHeap);
//

namespace Haku
{
namespace Renderer
{
class RenderCommandList
{
public:
	RenderCommandList(RenderDevice& device, CommandQueue& queue, D3D12_COMMAND_LIST_TYPE type);
	~RenderCommandList();
	void					   Reset() noexcept;
	void					   Shutdown() noexcept;
	ID3D12GraphicsCommandList* Get() noexcept { return m_CommandList; };

	// These functions are expected to change as the interface develops
	void SetRootSignature(D3D12RootSignature& signature);
	void SetScissorRects(D3D12_RECT rect, uint32_t rectnum);
	void SetViewports(D3D12_VIEWPORT port, uint32_t viewportnum = 1u);
	void SetPipelineState(CommandQueue& allocator, D3D12PipelineState& state);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//void SetDescriptorHeap();
	
	void TransitionBarriers(
		ID3D12Resource*				 Resource,
		D3D12_RESOURCE_STATES		 statebefore,
		D3D12_RESOURCE_STATES		 stateafter,
		D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
	// void Render()...???

private:
	ID3D12GraphicsCommandList* m_CommandList;
};
}; // namespace Renderer
}; // namespace Haku
