#pragma once
#include "Events.hpp"
#include "../Defines.hpp"
#include "../Platform/Windows/MainWindow.hpp"

namespace Haku
{
class HAKU_API Application
{
public:
	Application() = default;
	void SetMainWindow(Windows& window) noexcept;
	/*might get redefined to return exit code*/
	void ProcessMessage();
	void OnEvent(Event Event);

private:
	bool Onclose(Event& Close);
	void DispatchEvent();
/*stud event to handle the unmapped event for the time being*/
	bool return_true(Event& event) { return true; }

private:
	bool			m_Running = true;
	EventDispatcher Dispatcher;
	Windows*		m_Window = nullptr;
};
} // namespace Haku
