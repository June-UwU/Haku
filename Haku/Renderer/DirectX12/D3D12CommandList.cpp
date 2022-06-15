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
void CommandList::Reset()
{
	auto select_uint = m_Select.fetch_add(1u, std::memory_order_seq_cst);
	auto select		 = select_uint % FrameCount;

	// auto ret = m_CommandList->Close();
	// HAKU_SOK_ASSERT(ret)
	auto ret = m_Allocator[select]->Reset();
	HAKU_SOK_ASSERT(ret)
	ret = m_CommandList->Reset(m_Allocator[select], nullptr);
	HAKU_SOK_ASSERT(ret);
}
void CommandList::Release()
{
	for (auto* ptr : m_Allocator)
	{
		ptr->Release();
	}
	m_CommandList->Release();
}

void ResetStaleList()
{
	CommandList* list = nullptr;
	auto size = S_StaleCommandList.size();
	while (size > FrameCount)
	{
		list = S_StaleCommandList.front();
		switch (list->m_Type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			S_DirectCommandList.push_back(list);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			S_ComputeCommandList.push_back(list);
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			S_CopyCommandList.push_back(list);
			break;
		}
		default:
		{
			HAKU_LOG_CRIT("Unknown Command List Type In Stale List TYPE : ", list->m_Type);
			HAKU_THROW("Unknown Command List Type In Stale List ");
			break;
		}
		}
	}
}
void ReturnStaleList(CommandList* ptr)
{
	S_StaleCommandList.push_back(ptr);
}
CommandList* RequestCommandList(D3D12_COMMAND_LIST_TYPE type)
{
	CommandList* ret_val = nullptr;
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	{
		if (!S_DirectCommandList.empty())
		{
			ret_val = S_DirectCommandList.front();
			S_DirectCommandList.pop_front();
		}
		else
		{
			HAKU_LOG_INFO("No Available CommandList, Allocating a new list TYPE : ", type);
			ret_val = CreateCommandList(type);
		}
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	{
		if (!S_DirectCommandList.empty())
		{
			ret_val = S_ComputeCommandList.front();
			S_ComputeCommandList.pop_front();
		}
		else
		{
			HAKU_LOG_INFO("No Available CommandList, Allocating a new list TYPE : ", type);
			ret_val = CreateCommandList(type);
		}
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COPY:
	{
		if (!S_DirectCommandList.empty())
		{
			ret_val = S_CopyCommandList.front();
			S_CopyCommandList.pop_front();
		}
		else
		{
			HAKU_LOG_INFO("No Available CommandList, Allocating a new list TYPE : ", type);
			ret_val = CreateCommandList(type);
		}
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
	CommandList* list = nullptr;
	while (!S_StaleCommandList.empty())
	{
		list = S_StaleCommandList.front();
		S_StaleCommandList.pop_front();
		list->Release();
	}
	while (!S_DirectCommandList.empty())
	{
		list = S_DirectCommandList.front();
		S_DirectCommandList.pop_front();
		list->Release();
	}
	while (!S_ComputeCommandList.empty())
	{
		list = S_ComputeCommandList.front();
		S_ComputeCommandList.pop_front();
		list->Release();
	}
	while (!S_CopyCommandList.empty())
	{
		list = S_CopyCommandList.front();
		S_CopyCommandList.pop_front();
		list->Release();
	}
}
} // namespace Renderer
} // namespace Haku
