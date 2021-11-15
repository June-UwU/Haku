#include "../../Defines.hpp"
#include "../../Core/Window.hpp"
#include "wil/resource.h"
#include <Windows.h>

namespace Haku
{
class HAKU_API Windows : public Window
{
public:
	Windows(uint32_t height, uint32_t width, bool maximize, const char* Windowname);
	virtual void   run() override;
	static LRESULT ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
	wil::unique_hwnd handle;
};
} // namespace Haku
