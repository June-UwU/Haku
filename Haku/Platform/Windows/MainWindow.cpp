#include "MainWindow.hpp"

namespace Haku
{
static LRESULT CALLBACK WindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	// CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
	Windows* HandleWindow = reinterpret_cast<Windows*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	return HandleWindow->ProcessMessage(handle, message, wParam, lParam);
};

Windows::Windows(uint32_t height, uint32_t width, bool maximize, const char* Windowname)
	: Window(height, width, maximize, Windowname)
{
	HINSTANCE	Instance = GetModuleHandleA(nullptr);
	WNDCLASSEXA WindowClass{};
	WindowClass.cbSize		  = sizeof(WNDCLASSEXA);
	WindowClass.lpfnWndProc	  = WindowProc;
	WindowClass.hInstance	  = Instance;
	WindowClass.lpszClassName = Windowname;
	RegisterClassExA(&WindowClass);
	DWORD styles = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_EX_LAYERED;
	if (maximize)
	{
		styles |= WS_MAXIMIZE;
	}

	handle.reset(CreateWindowExA(
		0,
		Windowname,
		"Haku Engine",
		styles,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,
		nullptr,
		Instance,
		this));
	SetWindowLongPtr(handle.get(), GWLP_USERDATA, (LONG_PTR)this);
}

void Windows::run()
{
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT Windows::ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret{};
	switch (message)
	{
	case WM_QUIT:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	default:
		return DefWindowProcA(handle, message, wParam, lParam);
	}
	return ret;
}
} // namespace Haku
