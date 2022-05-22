#include "HakuLog.hpp"
#include "Application.hpp"

namespace Haku
{
Application* Application::s_Instance = nullptr;

Application::Application()
{
	HAKU_LOG_INFO("Creating Application");
	HAKU_ASSERT(!s_Instance);
	s_Instance = this;
	Dispatcher.RegisterRoutine(
		HAKU_BIND_FUNC(Application::Onclose), static_cast<uint32_t>(EventType::WindowCloseEvent));
	Dispatcher.RegisterRoutine(
		HAKU_BIND_FUNC(Application::OnResize), HAKU_EVENT_OR(EventType::WindowResizeEvent, EventType::Recurring));
	Dispatcher.RegisterRoutine(
		HAKU_BIND_FUNC(Application::OnMinimize), static_cast<uint32_t>(EventType::WindowMinimizeEvent));
}
void Application::SetMainWindow(Windows& window) noexcept
{
	HAKU_LOG_INFO("Setting Window");
	m_Window = &window;
	Haku::Renderer::RenderEngine::Initialize();
	m_Window->SetEventRoutine(HAKU_BIND_FUNC(OnEvent));
}
void Application::ProcessMessage()
{
	while (m_Running)
	{
		m_Window->run();
		Dispatcher.ServiceEvent();
		if (!m_Window->GetMinimize())
		{
			ClientUpdate();
		}
	}
	ClientCleanUp();
}
void Application::OnEvent(Event Event)
{
	Dispatcher.OnEvent(Event);
}

void Application::Onclose(Event& Event)
{
	HAKU_LOG_INFO();
	m_Running	  = false;
	Event.Handled = true;
}

void Application::OnResize(Event& event)
{
	HAKU_LOG_INFO();
	uint32_t width{};
	uint32_t height{};
	HAKU_DISPLAY_SIZE_EXTRACT(event.GetData(), height, width)
	m_Window->SetWidth(width);
	m_Window->SetHeight(height);
	m_Window->SetMinimize(false);
	//m_Renderer.Resize(height, width);
	event.Handled = true;
}

void Application::OnMinimize(Event& event)
{
	HAKU_LOG_INFO();
	HAKU_ASSERT(m_Window);
	event.Handled = true;
	m_Window->SetMinimize(true);
}

} // namespace Haku
