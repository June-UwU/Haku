#include "D3D12CommandList.hpp"
#include "D3D12Renderer.hpp"

namespace Haku
{
namespace Renderer
{
CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type)
	: m_Type(type)
{
	auto Device = RenderEngine::GetDeviceD3D();
	for (size_t i = 0; i < m_Allocator.size(); i++)
	{
		HAKU_SOK_ASSERT(Device->CreateCommandAllocator(
			m_Type, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&m_Allocator[i])))
	}
	HAKU_SOK_ASSERT(Device->CreateCommandList(
		0, m_Type, m_Allocator[0], nullptr, __uuidof(ID3D12CommandList), reinterpret_cast<void**>(&m_CommandList)))
	HAKU_SOK_ASSERT(m_CommandList->Close())
}
void CommandList::OMSetRenderTargets(
	UINT							   NumRenderTargetDescriptors,
	const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors,
	BOOL							   RTsSingleHandleToDescriptorRange,
	const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
{
	m_CommandList->OMSetRenderTargets(
		NumRenderTargetDescriptors,
		pRenderTargetDescriptors,
		RTsSingleHandleToDescriptorRange,
		pDepthStencilDescriptor);
}
void CommandList::RSSetViewports(UINT NumViewports, const D3D12_VIEWPORT* pViewports) noexcept
{
	m_CommandList->RSSetViewports(NumViewports, pViewports);
}
void CommandList::RSSetScissorRects(UINT NumRects, const D3D12_RECT* pRects) noexcept
{
	m_CommandList->RSSetScissorRects(NumRects, pRects);
}
void CommandList::IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) noexcept
{
	m_CommandList->IASetPrimitiveTopology(PrimitiveTopology);
}
void CommandList::ClearRenderTargetView(
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView,
	const float					ColorRGBA[4],
	UINT						NumRects,
	const D3D12_RECT*			pRects)
{
	m_CommandList->ClearRenderTargetView(RenderTargetView, ColorRGBA, NumRects, pRects);
}
void CommandList::SetPipelineState(PipeLineState* PSO)
{
	m_PSO = PSO;
	m_CommandList->SetPipelineState(PSO->Get());
}
void CommandList::Reset()
{
	auto select = m_Select % FrameCount;
	m_Select++;
	// auto ret = m_CommandList->Close();
	// HAKU_SOK_ASSERT(ret)
	auto ret = m_Allocator[select]->Reset();
	HAKU_SOK_ASSERT(ret)
	ret = m_CommandList->Reset(m_Allocator[select], nullptr);
	HAKU_SOK_ASSERT(ret);
}
void CommandList::Close()
{
	HAKU_SOK_ASSERT(m_CommandList->Close())
}
void CommandList::Release()
{
	for (auto* ptr : m_Allocator)
	{
		ptr->Release();
	}
	m_CommandList->Release();
}

void CommandList::TransitionSwapChainPresent()
{
	auto swapChain = RenderEngine::GetSwapChain();
	auto esbar	   = CD3DX12_RESOURCE_BARRIER::Transition(
		swapChain->GetRenderTargetResource(swapChain->GetFrameIndex()),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &esbar);
}

void CommandList::TransitionSwapChainRenderTarget()
{
	auto swapChain = RenderEngine::GetSwapChain();
	auto resbar	   = CD3DX12_RESOURCE_BARRIER::Transition(
		   swapChain->GetRenderTargetResource(swapChain->GetFrameIndex()),
		   D3D12_RESOURCE_STATE_PRESENT,
		   D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &resbar);
}

CommandList* RequestCommandList(D3D12_COMMAND_LIST_TYPE type)
{
	CommandList* ret_val = nullptr;
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	{
		ret_val = S_DirectList[S_DirectModVariable % FrameCount];
		S_DirectModVariable++;
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	{
		ret_val = S_ComputeList[S_ComputeModVariable % FrameCount];
		S_ComputeModVariable++;
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COPY:
	{
		ret_val = S_CopyList[S_CopyModVariable % FrameCount];
		S_CopyModVariable++;
		break;
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Command List Type In Stale List TYPE : ", type);
		HAKU_THROW("Unknown Command List Type In Stale List ");
		break;
	}
	}
	return ret_val;
}
CommandList* CreateCommandList(D3D12_COMMAND_LIST_TYPE type)
{
	return new CommandList(type);
}
void ShutDownCommandList()
{
	for (auto ptr : S_CopyList)
	{
		ptr->Release();
	}
	for (auto ptr : S_DirectList)
	{
		ptr->Release();
	}
	for (auto ptr : S_ComputeList)
	{
		ptr->Release();
	}
}
void InitializeCommandListArrays()
{
	for (auto& ptr : S_CopyList)
	{
		ptr = CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
	}
	for (auto& ptr : S_DirectList)
	{
		ptr = CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	for (auto& ptr : S_ComputeList)
	{
		ptr = CreateCommandList(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	}
}
} // namespace Renderer
} // namespace Haku
