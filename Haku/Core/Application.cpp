#include "HakuLog.hpp"
#include "Application.hpp"

namespace Haku
{
void Application::SetMainWindow(Windows& window) noexcept
{
	int x = 5;
	HAKU_LOG_INFO("poggers", "poggy",x);
	HAKU_LOG_WARN("warn", x);
	HAKU_LOG_CRIT("critical", x);
	HAKU_LOG_ERROR("error", x);
	p_Window = &window;
}
void Application::ProcessMessage()
{
	p_Window->run();
}
} // namespace Haku
