#pragma once
#include "macros.hpp"
#include "HakuLog.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "Utils/Dequeue.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12PipelineState.hpp"
#include "GPUVisibleDescriptorHeap.hpp"

namespace Haku
{
namespace Renderer
{
class CommandList
{
	using PSO = PipeLineState;

public:
	CommandList(D3D12_COMMAND_LIST_TYPE type);
	DISABLE_COPY(CommandList)
	DISABLE_MOVE(CommandList)

	void Reset();
	void Close();
	void Release();
	void TransitionSwapChainPresent();
	void TransitionSwapChainRenderTarget();
	void SetPipelineState(PipeLineState* PSO);
	void ClearRenderTargetView(
		D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView,
		const float					ColorRGBA[4],
		UINT						NumRects,
		const D3D12_RECT*			pRects);
	void OMSetRenderTargets(
		UINT							   NumRenderTargetDescriptors,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors,
		BOOL							   RTsSingleHandleToDescriptorRange,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor);
	void RSSetViewports(UINT NumViewports, const D3D12_VIEWPORT* pViewports) noexcept;
	void RSSetScissorRects(UINT NumRects, const D3D12_RECT* pRects) noexcept;
	void IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) noexcept;

public:
	D3D12_COMMAND_LIST_TYPE	   m_Type;
	ID3D12GraphicsCommandList* m_CommandList;

private:
	std::array<ID3D12CommandAllocator*, FrameCount> m_Allocator;
	uint64_t										m_Select = 0;
	PSO*											m_PSO	 = nullptr;
};

static uint64_t S_CopyModVariable;
static uint64_t S_DirectModVariable;
static uint64_t S_ComputeModVariable;

static std::array<CommandList*, FrameCount> S_CopyList;
static std::array<CommandList*, FrameCount> S_DirectList;
static std::array<CommandList*, FrameCount> S_ComputeList;

void		 ShutDownCommandList();
void		 InitializeCommandListArrays();
CommandList* CreateCommandList(D3D12_COMMAND_LIST_TYPE type);
CommandList* RequestCommandList(D3D12_COMMAND_LIST_TYPE type);
} // namespace Renderer
} // namespace Haku
