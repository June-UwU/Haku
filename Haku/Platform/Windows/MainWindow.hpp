#include "wil/resource.h"
#include "../../Defines.hpp"
#include "../../Core/Window.hpp"
#include <Windows.h>

//	using EventRoutine = std::function<void(Event& e)>;
//
//	EventRoutine Routine;
//
//	public:
//		virtual void run() = 0;
//		virtual void SetEventRoutine(const EventRoutine& func) = 0;
//

namespace Haku
{
class HAKU_API Windows : public Window
{
public:
	Windows(uint32_t height, uint32_t width, bool maximize, const char* Windowname);
	void	run() override;
	void	SetEventRoutine(const EventRoutine& func) override;
	LRESULT ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
	wil::unique_hwnd handle;
	wil::unique_hicon icon;
};
} // namespace Haku
