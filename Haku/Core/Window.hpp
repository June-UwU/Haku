#include "../Defines.hpp"
#include <Windows.h>
#include "wil/resource.h"

namespace Haku
{
class HAKU_API Window
{
	friend class Application;

public:
	Window(uint32_t height, uint32_t width, bool maximize, const char* Windowname);
	void run();
	static LRESULT ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
private:
	std::uint32_t height;
	std::uint32_t width;
	bool		  maximize;
	/*needs to be abstracted*/
	wil::unique_hwnd handle;
};
} // namespace Haku
