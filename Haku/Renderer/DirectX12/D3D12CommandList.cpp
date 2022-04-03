#include "D3D12CommandList.hpp"
namespace Haku
{
namespace Renderer
{
RenderCommandList::RenderCommandList(RenderDevice& device, CommandQueue& queue, D3D12_COMMAND_LIST_TYPE type)
{
	HAKU_SOK_ASSERT(
		device.Get()->CreateCommandList(0, type, queue.GetAllocator(), nullptr, IID_PPV_ARGS(&m_CommandList)));
}
RenderCommandList::~RenderCommandList()
{
	Shutdown();
}

void RenderCommandList::Shutdown() noexcept
{
	m_CommandList->Release();
}
void RenderCommandList::SetViewports(D3D12_VIEWPORT port, uint32_t viewportnum)
{
	m_CommandList->RSSetViewports(viewportnum, &port);
}
void RenderCommandList::SetPipelineState(CommandQueue& allocator, D3D12PipelineState& state)
{
	m_CommandList->SetPipelineState(state.Get());
}
void RenderCommandList::SetScissorRects(D3D12_RECT rect, uint32_t rectnum)
{
	m_CommandList->RSSetScissorRects(rectnum, &rect);
}
void RenderCommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	m_CommandList->IASetPrimitiveTopology(topology);
}
void RenderCommandList::SetRootSignature(D3D12RootSignature& signature)
{
	m_CommandList->SetGraphicsRootSignature(signature.Get());
}
//void RenderCommandList::SetPipelineState(D3D12CommandQueue& allocator, D3D12PipelineState& state)
//{
//	m_CommandList->Reset(allocator.GetCommandAllocator(), state.Get());
//}

void RenderCommandList::TransitionBarriers(
	ID3D12Resource*				 resource,
	D3D12_RESOURCE_STATES		 statebefore,
	D3D12_RESOURCE_STATES		 stateafter,
	D3D12_RESOURCE_BARRIER_FLAGS flags)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Flags				   = flags;
	barrier.Transition.pResource   = resource;
	barrier.Transition.StateAfter  = stateafter;
	barrier.Transition.StateBefore = statebefore;
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//do i need to support multiple barriers for easy use ..???
	m_CommandList->ResourceBarrier(1, &barrier);
}
void RenderCommandList::Reset() noexcept
{
	m_CommandList->Release();
}
} // namespace Renderer
} // namespace Haku
