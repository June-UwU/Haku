#include "HakuLog.hpp"
#include "Application.hpp"

#define HAKU_BIND_FUNC(x)                                                                                              \
	[this](auto&&... args) -> decltype(auto)                                                                           \
	{                                                                                                                  \
		return this->x(std::forward<decltype(args)>(args)...);                                                         \
	}

namespace Haku
{
void Application::SetMainWindow(Windows& window) noexcept
{
	HAKU_LOG_INFO(__FUNCTION__, "Setting Window");
	m_Window = &window;

	// need to learn perfect forwarding

	m_Window->SetEventRoutine(HAKU_BIND_FUNC(OnEvent));
}
void Application::ProcessMessage()
{
	while (m_Running)
	{
		m_Window->run();
		DispatchEvent();
	}
}
void Application::OnEvent(Event Event)
{
	Dispatcher.OnEvent(Event);
}
bool Application::Onclose(Event& Close)
{
	HAKU_LOG_INFO(__FUNCTION__);
	m_Running = false;
	return true;
}
void Application::DispatchEvent()
{
	if (!Dispatcher.empty())
	{
		switch (Dispatcher.peek().GetEventType())
		{
		case EventType::WindowCloseEvent:
		{
			Dispatcher.ServiceEvent(HAKU_BIND_FUNC(Application::Onclose));
			break;
		}
		default:
		{
			Dispatcher.ServiceEvent(HAKU_BIND_FUNC(Application::return_true));
		}
		}
	}
}
} // namespace Haku
