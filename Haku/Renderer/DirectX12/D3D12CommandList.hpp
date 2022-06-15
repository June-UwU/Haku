#pragma once
#include "macros.hpp"
#include "HakuLog.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "Utils/Dequeue.hpp"

namespace Haku
{
namespace Renderer
{
class CommandList
{
public:
	CommandList(D3D12_COMMAND_LIST_TYPE type);
	DISABLE_COPY(CommandList)
	DISABLE_MOVE(CommandList)

	void Reset();
	void Release();

public:
	D3D12_COMMAND_LIST_TYPE	   m_Type;
	ID3D12GraphicsCommandList* m_CommandList;

private:
	std::array<ID3D12CommandAllocator*, FrameCount> m_Allocator;
	std::atomic_uint64_t							m_Select = 0;
};

static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_CopyCommandList;
static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_DirectCommandList;
static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_ComputeCommandList;
static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_StaleCommandList;
void											ResetStaleList();
void											ReturnStaleList(CommandList* ptr);
CommandList*									RequestCommandList(D3D12_COMMAND_LIST_TYPE type);
CommandList*									CreateCommandList(D3D12_COMMAND_LIST_TYPE type);
void											ShutDownCommandList();
} // namespace Renderer
} // namespace Haku
