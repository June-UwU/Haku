#include "D3D12CommandList.hpp"
#include "D3D12Renderer.hpp"

namespace Haku
{
namespace Renderer
{
CommandList* CreateCommandList(D3D12_COMMAND_LIST_TYPE type)
{
	HAKU_LOG_INFO("Creating Command list Type : ", type);
	CommandList* ret_val = new CommandList;
	auto		 Device	 = RenderEngine::GetDeviceD3D();
	HAKU_SOK_ASSERT(Device->CreateCommandAllocator(
		type, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&ret_val->m_Allocator)))
	HAKU_SOK_ASSERT(Device->CreateCommandList(
		0,
		type,
		ret_val->m_Allocator,
		nullptr,
		__uuidof(ID3D12CommandList),
		reinterpret_cast<void**>(&ret_val->m_CommandList)))
	ret_val->m_Type = type;
	return ret_val;
}
CommandList* RequestCommandList(D3D12_COMMAND_LIST_TYPE type)
{
	CommandList* ret_val = nullptr;
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	{
		if (!S_ReadyDirectCommandList.empty())
		{
			ret_val = S_ReadyDirectCommandList.front();
			S_ReadyDirectCommandList.pop_front();
		}
		else
		{
			HAKU_LOG_INFO("No  Ready Commandlist Allocating a new Commandlist");
			ret_val = CreateCommandList(type);
		}
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	{
		if (!S_ReadyComputeCommandList.empty())
		{
			ret_val = S_ReadyComputeCommandList.front();
			S_ReadyComputeCommandList.pop_front();
		}
		else
		{
			HAKU_LOG_INFO("No  Ready Commandlist Allocating a new Commandlist");
			ret_val = CreateCommandList(type);
		}
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COPY:
	{
		if (!S_ReadyCopyCommandList.empty())
		{
			ret_val = S_ReadyCopyCommandList.front();
			S_ReadyCopyCommandList.pop_front();
		}
		else
		{
			HAKU_LOG_INFO("No  Ready Commandlist Allocating a new Commandlist");
			ret_val = CreateCommandList(type);
		}
		break;
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Type of Command List", type);
		HAKU_THROW("Unknown Type of Command List");
		break;
	}
	}
	return ret_val;
}
void ReturnStaleList(CommandList* ptr)
{
	S_StaleCommandList.push_back(ptr);
}
void RepurposeStaleList()
{
	while (!S_StaleCommandList.empty())
	{
		auto commandlist = S_StaleCommandList.front();
		switch (commandlist->m_Type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			S_ReadyDirectCommandList.push_back(commandlist);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			S_ReadyComputeCommandList.push_back(commandlist);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			S_ReadyCopyCommandList.push_back(commandlist);
			break;
		}
		default:
		{
			HAKU_LOG_CRIT("Unknown Type of Command List In Stale Queue", commandlist->m_Type);
			HAKU_THROW("Unknown Type of Command List In Stale Queue");
			break;
		}
		}
		S_StaleCommandList.pop_front();
		commandlist->Reset();
	}
}
void ShutDownCommandlist()
{
	HAKU_LOG_INFO("Releasing Command List Pool");
	CommandList* list = nullptr;
	while (!S_StaleCommandList.empty())
	{
		list = S_StaleCommandList.front();
		S_StaleCommandList.pop_front();
		list->Release();
		delete list;
	}
	while (!S_ReadyCopyCommandList.empty())
	{
		list = S_ReadyCopyCommandList.front();
		S_ReadyCopyCommandList.pop_front();
		list->Release();
		delete list;
	}
	while (!S_ReadyDirectCommandList.empty())
	{
		list = S_ReadyDirectCommandList.front();
		S_ReadyDirectCommandList.pop_front();
		list->Release();
		delete list;
	}
	while (!S_ReadyComputeCommandList.empty())
	{
		list = S_ReadyComputeCommandList.front();
		S_ReadyComputeCommandList.pop_front();
		list->Release();
		delete list;
	}
}
void CommandList::Reset()
{
	ResetCommandAllocator();
	ResetCommandList();
}
void CommandList::Release()
{
	m_Allocator->Release();
	m_CommandList->Release();
}
void CommandList::ResetCommandList()
{
	m_CommandList->Reset(m_Allocator, nullptr);
}
void CommandList::ResetCommandAllocator()
{
	m_Allocator->Reset();
}
} // namespace Renderer
} // namespace Haku
