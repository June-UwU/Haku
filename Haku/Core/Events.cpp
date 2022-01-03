#include "Events.hpp"

namespace Haku
{
void EventDispatcher::ServiceEvent()
{
	
	while (!m_Queue.empty())
	{
		
		auto result = m_CallBack.find(static_cast<uint32_t>(m_Queue.front().GetEventType()));
		if (result == m_CallBack.end())
		{
			m_Queue.pop();
			continue;
		}
		auto range = m_CallBack.equal_range(static_cast<uint32_t>(m_Queue.front().GetEventType()));
		for (auto it = range.first; it != range.second; ++it)
		{
			std::invoke(it->second, m_Queue.front());
			if (m_Queue.front().Handled)
			{
				m_Queue.pop();
				break;
			}
		}
	}
}
void EventDispatcher::RegisterRoutine(Routine r, uint32_t type)
{
	m_CallBack.insert(std::make_pair(type, r));
}
void EventDispatcher::OnEvent(Event& e) noexcept
{
	while (m_Queue.size() > 16u)
	{
		m_Queue.pop();
	}
	m_Queue.push(e);
}
} // namespace Haku
