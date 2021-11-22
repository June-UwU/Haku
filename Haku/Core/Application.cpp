#include "HakuLog.hpp"
#include "Application.hpp"

namespace Haku
{
Application* Application::s_Instance = nullptr;

Application::Application()
{
	HAKU_ASSERT(!s_Instance);
	s_Instance = this;
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
		DispatchEvent();
	}
}
void Application::OnEvent(Event Event)
{
	Dispatcher.OnEvent(Event);
}
void Application::AddLayer(Layer* layer)
{
	m_LayerStack.Addlayer(layer);
}
void Application::AddOverlay(Layer* layer)
{
	m_LayerStack.AddOverLay(layer);
}
void Application::RemoveLayer(Layer* layer)
{
	m_LayerStack.RemoveLayer(layer);
}
void Application::RemoveOverlay(Layer* layer)
{
	m_LayerStack.RemoveOverlay(layer);
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
			Dispatcher.ServiceEvent(HAKU_BIND_FUNC(m_LayerStack.OnEvent));
			break;
		}
		default:
		{
			Dispatcher.ServiceEvent(HAKU_BIND_FUNC(m_LayerStack.OnEvent));
			Dispatcher.ServiceEvent(HAKU_BIND_FUNC(Application::return_true));
		}
		}
	}
}
} // namespace Haku
