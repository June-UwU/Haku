#include "Application.hpp"
#include "HakuLog.hpp"

namespace Haku
{
void Application::SetMainWindow(Windows& window) noexcept
{
	HAKU_LOG_INFO("setting window");
	p_Window = &window;
}
void Application::ProcessMessage()
{
	p_Window->run();
}
} // namespace Haku
