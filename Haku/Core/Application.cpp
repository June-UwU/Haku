#include "HakuLog.hpp"
#include "Application.hpp"

namespace Haku
{
Application* Application::s_Instance = nullptr;

Application::Application()
{
	HAKU_ASSERT(!s_Instance);
	s_Instance = this;
	Dispatcher.RegisterRoutine(
		HAKU_BIND_FUNC(Application::Onclose), static_cast<uint32_t>(EventType::WindowCloseEvent));
}
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
		Dispatcher.ServiceEvent();
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

} // namespace Haku
