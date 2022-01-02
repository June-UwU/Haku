#pragma once
#include "wil/resource.h"
#include "../../macros.hpp"
#include "../../hakupch.hpp"
#include "../../Core/Window.hpp"

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
	HWND	GetHandle() { return handle.get(); }
	void	SetEventRoutine(const EventRoutine& func) override;
	LRESULT ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	wil::unique_hicon icon;
	wil::unique_hwnd  handle;
};
} // namespace Haku
