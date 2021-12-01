#include "HakuLog.hpp"
#include "Application.hpp"

namespace Haku
{
Application* Application::s_Instance = nullptr;

Application::Application()
	: m_Renderer(720, 1080)
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

	m_Renderer.Init(m_Window);
	// need to learn perfect forwarding
	m_Window->SetEventRoutine(HAKU_BIND_FUNC(OnEvent));
}
void Application::ProcessMessage()
{
	while (m_Running)
	{
		m_Window->run();
		Dispatcher.ServiceEvent();
		m_Renderer.Render();
	}
}
void Application::OnEvent(Event Event)
{
	Dispatcher.OnEvent(Event);
}

void Application::Onclose(Event& Close)
{
	HAKU_LOG_INFO(__FUNCTION__);
	Close.Handled = true;
	m_Running	  = false;
}

} // namespace Haku
