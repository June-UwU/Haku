#pragma once
#include "HakuLog.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "Utils/Dequeue.hpp"

namespace Haku
{
namespace Renderer
{
struct CommandList
{
	void					   Reset();
	void					   Release();
	void					   ResetCommandList();
	void					   ResetCommandAllocator();
	D3D12_COMMAND_LIST_TYPE	   m_Type;
	ID3D12CommandAllocator*	   m_Allocator;
	ID3D12GraphicsCommandList* m_CommandList;
};

class ProxyCommandList
{
public:
private:
};

static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_ReadyCopyCommandList;
static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_ReadyDirectCommandList;
static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_ReadyComputeCommandList;
static Haku::Utils::Hk_Dequeue_mt<CommandList*> S_StaleCommandList;

CommandList* CreateCommandList(D3D12_COMMAND_LIST_TYPE type);
CommandList* RequestCommandList(D3D12_COMMAND_LIST_TYPE type);
void		 ReturnStaleList(CommandList* ptr);
void		 RepurposeStaleList();
void		 ShutDownCommandlist();
} // namespace Renderer
} // namespace Haku
