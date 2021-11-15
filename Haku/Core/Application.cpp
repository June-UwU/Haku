#include "Application.hpp"

namespace Haku
{
	void Application::SetMainWindow(Windows& window) noexcept
	{
		p_Window = &window;
	}
	void Application::ProcessMessage()
	{
		p_Window->run();

	}
}
