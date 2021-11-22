#pragma once
#include "LayerStack.hpp"
#include "Events.hpp"
#include "../macros.hpp"
#include "../Platform/Windows/MainWindow.hpp"

namespace Haku
{
class HAKU_API Application
{
public:
	Application();
	void SetMainWindow(Windows& window) noexcept;
	/*might get redefined to return exit code*/
	void ProcessMessage();
	void OnEvent(Event Event);
	void AddLayer(Layer* layer);
	void AddOverlay(Layer* layer);
	void RemoveLayer(Layer* layer);
	void RemoveOverlay(Layer* layer);

private:
	bool Onclose(Event& Close);
	void DispatchEvent();
	/*stud event to handle the unmapped event for the time being*/
	bool return_true(Event& event)
	{
		HAKU_LOG_WARN(__FUNCTION__);
		return true;
	}

private:
	EventDispatcher Dispatcher;
	LayerStack		m_LayerStack;
	bool			m_Running = true;
	Windows*		m_Window  = nullptr;

private:
	static Application* s_Instance;
};
} // namespace Haku
