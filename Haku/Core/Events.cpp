#include "Events.hpp"

namespace Haku
{
void EventDispatcher::ServiceEvent()
{
	while (!m_Queue.empty())
	{
		auto range = m_CallBack.equal_range(static_cast<uint32_t>(m_Queue.front().GetEventType()));
		for (auto it = range.first; it != range.second; ++it)
		{
			std::invoke(it->second, m_Queue.front());
			if (m_Queue.front().Handled)
			{
				m_Queue.pop_front();
				break;
			}
		}
		/*this is temp  for the handling of event*/
		m_Queue.pop_front();
	}
}
void EventDispatcher::RegisterRoutine(Routine r, uint32_t type)
{
	m_CallBack.insert(std::make_pair(type, r));
}
} // namespace Haku
