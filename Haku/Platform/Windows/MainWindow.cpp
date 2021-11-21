#include "MainWindow.hpp"
#include "../../Core/Events.hpp"
#include "../../Core/HakuLog.hpp"
#include <filesystem>
#include <WinUser.h>

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
	char Filepath[256];
	HAKU_LOG_INIT();
	HAKU_LOG_INFO("Haku chan : hellow... ^_^");
	HAKU_LOG_INFO(__FUNCTION__, "Windows creation");
	GetModuleFileName(nullptr, Filepath, std::size(Filepath));

	std::filesystem::path ExecutablePath(Filepath);
	auto				  IconPath = ExecutablePath.parent_path() / "../Haku.ico";
	icon.reset(reinterpret_cast<HICON>(
		LoadImage(nullptr, IconPath.string().data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE)));
	HINSTANCE	Instance = GetModuleHandleA(nullptr);
	WNDCLASSEXA WindowClass{};
	WindowClass.cbSize		  = sizeof(WNDCLASSEXA);
	WindowClass.lpfnWndProc	  = WindowProc;
	WindowClass.hInstance	  = Instance;
	WindowClass.lpszClassName = Windowname;
	WindowClass.hIcon		  = icon.get();
	RegisterClassExA(&WindowClass);
	DWORD styles = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_EX_LAYERED;
	if (maximize)
	{
		HAKU_LOG_INFO(__FUNCTION__, "Style Maximum :", maximize);
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
	if (!handle.get())
	{
		HAKU_LOG_CRIT(__FUNCTION__, "No Windows Handle returned ");
	}
	SetWindowLongPtr(handle.get(), GWLP_USERDATA, (LONG_PTR)this);
}

void Windows::run()
{
	// need to change to peek message
	MSG msg = {};
	while (PeekMessageA(&msg, handle.get(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Windows::SetEventRoutine(const EventRoutine& func)
{
	HAKU_LOG_INFO(__FUNCTION__);
	Routine = func;
}

LRESULT Windows::ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret{};
	switch (message)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		Window::Routine(HAKU_WINDOW_EVENT(EventType::WindowCloseEvent, 0));
		break;
	}
	case WM_KEYDOWN:
	{
		Window::Routine(HAKU_KEY_EVENT(EventType::KeyDownEvent, wParam));
		break;
	}
	case WM_KEYUP:
	{
		Window::Routine(HAKU_KEY_EVENT(EventType::KeyUpEvent, wParam));
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseMoveEvent, data));
		break;
	}
	case WM_LBUTTONDOWN:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseLeftDownEvent, data));
		break;
	}
	case WM_LBUTTONUP:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseLeftUpEvent, data));
		break;
	}
	case WM_RBUTTONDOWN:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseRightDownEvent, data));
		break;
	}
	case WM_RBUTTONUP:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseRightUpEvent, data));
		break;
	}
	case WM_MBUTTONDOWN:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseMiddleDownEvent, data));
		break;
	}
	case WM_MBUTTONUP:
	{
		POINTS	points = MAKEPOINTS(lParam);
		int64_t data   = HAKU_MOUSE_PACK(data, points.x, points.y);
		Window::Routine(HAKU_MOUSE_EVENT(EventType::MouseMiddleUpEvent, data));
		break;
	}

	default:
		return DefWindowProcA(handle, message, wParam, lParam);
	}
	return ret;
}
} // namespace Haku
