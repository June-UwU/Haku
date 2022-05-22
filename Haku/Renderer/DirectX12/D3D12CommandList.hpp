#pragma once
#include <memory>
#include "macros.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "Core/Exceptions.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12PipelineState.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12ResourceStateTracker.hpp"

// TODO : functions to port over
// m_Signature->SetSignature(*m_Command);
// m_DescriptorHeap->SetDescriptorHeaps(*m_Command);
// These two are probably commandlist methods
// void ClearRenderTargets() noexcept;
/// would probably thanos this function
// void TransitionPresent();

namespace Haku
{
namespace Renderer
{
struct D3DCommandList
{
	D3DCommandList(D3D12_COMMAND_LIST_TYPE type);
	void Reset() noexcept;
	void ShutDown() noexcept;
	~D3DCommandList();

	D3D12_COMMAND_LIST_TYPE	   m_Type;
	ID3D12CommandAllocator*	   m_CommandAllocator = nullptr;
	ID3D12GraphicsCommandList* m_CommandList	  = nullptr;
};
static D3DCommandList* CreateD3DCommandList(D3D12_COMMAND_LIST_TYPE type);

class CommandList
{
public:
	// CommandList(D3D12_COMMAND_LIST_TYPE type);
	//~CommandList();
	// void					   Reset() noexcept;
	// void					   Shutdown() noexcept;
	// ID3D12GraphicsCommandList* Get() noexcept { return m_CommandList; };
	//
	//// These functions are expected to change as the interface develops
	// void SetRootSignature(D3D12RootSignature& signature);
	// void SetScissorRects(D3D12_RECT rect, uint32_t rectnum);
	// void SetViewports(D3D12_VIEWPORT port, uint32_t viewportnum = 1u);
	// void SetPipelineState(CommandQueue& allocator, D3D12PipelineState& state);
	// void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	////void SetDescriptorHeap();
	//
	// void TransitionBarriers(
	//	ID3D12Resource*				 Resource,
	//	D3D12_RESOURCE_STATES		 statebefore,
	//	D3D12_RESOURCE_STATES		 stateafter,
	//	D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
	//// void Render()...???

private:
	std::vector<D3DCommandList*> m_ExecuteCommandList;

	static Utils::HK_Queue_mt<D3DCommandList*> m_FreeListCopy;
	static Utils::HK_Queue_mt<D3DCommandList*> m_FreeListDirect;
	static Utils::HK_Queue_mt<D3DCommandList*> m_FreeListCompute;

	static Utils::HK_Queue_mt<D3DCommandList*> m_StaleListCopy;
	static Utils::HK_Queue_mt<D3DCommandList*> m_StaleListDirect;
	static Utils::HK_Queue_mt<D3DCommandList*> m_StaleListCompute;
};

static CommandList* CreateCommandList();
}; // namespace Renderer
}; // namespace Haku
