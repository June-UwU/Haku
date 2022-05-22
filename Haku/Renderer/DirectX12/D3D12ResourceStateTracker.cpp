#include "D3D12ResourceStateTracker.hpp"
#include "Exceptions.hpp"

namespace Haku
{
namespace Renderer
{
std::unordered_map<ID3D12Resource*, ResourceState> ResourceStateTracker::m_StateMap;

void ResourceStateTracker::ResetLocalStructures() noexcept
{
	m_Barriers.clear();
	m_PendingBarriers.clear();
}

void ResourceStateTracker::FlushPendingBarriers()
{
	if (m_PendingBarriers.size()) // there are pending barriers that needs its before state set
	{
		for (int32_t i = m_PendingBarriers.size() - 1; i > -1; i--)
		{
			auto  ret_barrier = m_PendingBarriers[i];
			auto& transtion	  = ret_barrier.Transition;

			auto  state_return	 = m_StateMap.find(transtion.pResource);
			auto& resource_state = state_return->second;
			if (state_return == m_StateMap.end()) // this resource is not registered
			{
				HAKU_LOG_ERROR("Referencing a unknown resource for transition state");
				HAKU_THROW("Referencing a unknown resource for transition state")
			}
			else
			{
				if (transtion.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					resource_state.GeneralStateSet()) // case where all states are known
				{
					transtion.StateBefore = resource_state.GetGeneralState();
					m_Barriers.push_back(ret_barrier);
					resource_state.SetGeneralState(transtion.StateAfter);
				}
				else if (
					transtion.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					!resource_state
						 .GeneralStateSet()) // case where all the subresource needs to changed but has no general state
				{
					for (uint32_t i = 0; i <= resource_state.GetCount(); i++)
					{
						auto ret_state = resource_state[i];
						// checks if the state the resource is in is the state we want , if it is we don't do anything
						if (transtion.StateAfter != ret_state)
						{
							D3D12_RESOURCE_BARRIER barrier;
							barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
							barrier.Transition.pResource   = transtion.pResource;
							barrier.Transition.StateAfter  = transtion.StateAfter;
							barrier.Transition.Subresource = i;
							barrier.Transition.StateBefore = ret_state;

							m_Barriers.push_back(std::move(barrier));

							resource_state.SetSubResourceState(i, transtion.StateAfter);
						}
					}
				}
				else // single resource transformation case
				{
					if (resource_state[transtion.Subresource] != transtion.StateAfter)
					{
						transtion.StateBefore = resource_state[transtion.Subresource];
						resource_state.SetSubResourceState(transtion.Subresource, transtion.StateAfter);
						m_Barriers.push_back(ret_barrier);
					}
				}
			}
		}
		//clear the pending vector since it is all resolved
		m_PendingBarriers.clear();
	}
}

void ResourceStateTracker::TransitionBarriers(
	ID3D12Resource*		  resource,
	D3D12_RESOURCE_STATES state_after,
	uint32_t			  slot) noexcept
{
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource;
	barrier.Transition.StateAfter  = state_after;
	barrier.Transition.Subresource = slot;

	m_PendingBarriers.push_back(std::move(barrier));
}

void ResourceStateTracker::UnRegisterResource(ID3D12Resource* ptr)
{
	HAKU_LOG_INFO("Unregistering Resource ");
	auto result = m_StateMap.find(ptr);

	if (result == m_StateMap.end())
	{
		HAKU_LOG_CRIT("Unknown Resource Release....");
		HAKU_THROW("Unknown Resource Release");
	}

	m_StateMap.erase(result);
}
void ResourceStateTracker::RegisterResource(ID3D12Resource* ptr, const ResourceState& state) noexcept
{
	HAKU_LOG_INFO("Registering Resource  : LValue Reference");
	m_StateMap.emplace(ptr, ResourceState());
	m_StateMap[ptr] = state;
}

void ResourceStateTracker::RegisterResource(ID3D12Resource* ptr, const ResourceState&& state) noexcept
{
	HAKU_LOG_INFO("Registering Resource  : RValue Reference");
	m_StateMap.emplace(ptr, ResourceState());
	m_StateMap[ptr] = state;
}

ResourceState::ResourceState(const std::vector<D3D12_RESOURCE_STATES>& vector) noexcept
	: m_SubResourceCount(vector.size())
{
	for (uint32_t i = 0; i < vector.size(); i++)
	{
		m_StateMap.emplace(i, vector[i]);
	}
}

ResourceState::ResourceState(const D3D12_RESOURCE_STATES* ptr, size_t ptr_count) noexcept
	: m_SubResourceCount(ptr_count)
{
	for (size_t i = 0; i < ptr_count; i++)
	{
		m_StateMap.emplace(i, ptr[i]);
	}
}

ResourceState::ResourceState(uint32_t subresource_count, D3D12_RESOURCE_STATES state) noexcept
	: m_SubResourceCount(subresource_count)
{
	// entire state initializations
	m_StateMap.clear();
	m_GeneralState = state;
}

ResourceState::ResourceState(const ResourceState& ref) noexcept
	: m_SubResourceCount(ref.m_SubResourceCount)
{
	if (ref.m_StateMap.empty())
	{
		m_GeneralState = ref.m_GeneralState;
	}
	else
	{
		for (auto& val : ref.m_StateMap)
		{
			m_StateMap.emplace(val.first, val.second);
		}
	}
}

ResourceState::ResourceState(const ResourceState&& ref) noexcept
	: m_SubResourceCount(std::move(ref.m_SubResourceCount))
{
	if (ref.m_StateMap.empty())
	{
		m_GeneralState = std::move(ref.m_GeneralState);
	}
	else
	{
		m_StateMap = std::move(ref.m_StateMap);
	}
}

ResourceState& ResourceState::operator=(const ResourceState& ref) noexcept
{
	m_SubResourceCount = ref.m_SubResourceCount;
	if (ref.m_StateMap.empty())
	{
		m_GeneralState = ref.m_GeneralState;
	}
	else
	{
		for (auto& val : ref.m_StateMap)
		{
			m_StateMap.emplace(val.first, val.second);
		}
	}
	return *this;
}

ResourceState& ResourceState::operator=(const ResourceState&& ref) noexcept
{
	m_SubResourceCount = std::move(ref.m_SubResourceCount);
	if (ref.m_StateMap.empty())
	{
		m_GeneralState = std::move(ref.m_GeneralState);
	}
	else
	{
		m_StateMap = std::move(ref.m_StateMap);
	}
	return *this;
}

D3D12_RESOURCE_STATES ResourceState::operator[](uint32_t pos) const
{
	return m_StateMap.find(pos)->second;
}

const D3D12_RESOURCE_STATES ResourceState::GetSubResourceState(uint32_t pos)
{
	const auto ret_val = m_StateMap.find(pos);
	if (ret_val == m_StateMap.end())
	{
		HAKU_LOG_ERROR("GetSubResource : Out Of Bounds For SubResource State");
		HAKU_THROW("Out Of Bounds For SubResource State");
	}
	return ret_val->second;
}

void ResourceState::SetGeneralState(D3D12_RESOURCE_STATES state)
{
	m_GeneralState = state;
}

void ResourceState::SetSubResourceState(uint32_t pos, const D3D12_RESOURCE_STATES state)
{
	auto ret_val = m_StateMap.find(pos);
	if (ret_val == m_StateMap.end())
	{
		HAKU_LOG_ERROR("SetSubResourceState : Out Of Bounds For SubResource State");
		HAKU_THROW("Out Of Bounds For SubResource State")
	}
	ret_val->second = state;
}

} // namespace Renderer
} // namespace Haku
