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

private:
	Windows* p_Window = nullptr;
};
} // namespace Haku
