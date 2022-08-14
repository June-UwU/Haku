#include "platform.hpp"

#ifdef HWIN32

#include <stdlib.h>

#include <Windows.h>
#include <string.h>
#include "core\logger.hpp"
#include <sysinfoapi.h>

typedef struct platform_state
{
	HINSTANCE hinstance;
	HWND 	  hwnd;
}platform_state;

static platform_state* state_ptr;

static constexpr WORD platform_level_lookup[LOG_LVL_COUNT]
{
	BACKGROUND_RED | BACKGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_GREEN
};


static LRESULT win32_msg_proc(HWND handle ,UINT msg, WPARAM wparam, LPARAM lparam);


	// TODO : make use of state
i32 platform_initialize(void* state, const char* name, u32 x, u32 y, u32 height, u32  width)
{	
	state_ptr 		= (platform_state*)malloc(sizeof(platform_state));

	state_ptr->hinstance 	= GetModuleHandle(nullptr);

	WNDCLASSA wndcls{};
	wndcls.lpfnWndProc 	= win32_msg_proc;
	wndcls.hInstance 	= state_ptr->hinstance;
	wndcls.hIcon		= nullptr; //TODO : do our own icon
	wndcls.hCursor		= nullptr; // handles cursor
	wndcls.lpszClassName	= name;

	if(!RegisterClassA(&wndcls))
	{
		HLEMER("Failed to register class for win32 windows creation");
		MessageBoxA(nullptr,"Window Registration Fail",nullptr,MB_OK);
		return H_FAIL;	
	}

	DWORD styles = WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPED | WS_SYSMENU;

	state_ptr->hwnd = CreateWindowExA(0,name,name,styles,x,y,width,height,nullptr,nullptr,state_ptr->hinstance,nullptr);

	if(!state_ptr->hwnd)
	{
		HLEMER("Failed to create a window");
		MessageBoxA(nullptr,"Window creation failure",nullptr,MB_OK);
		return H_FAIL;
	}
	
	ShowWindow(state_ptr->hwnd,SW_SHOWDEFAULT);

	return H_OK;
}

void platform_shutdown()
{
}

i8 platform_pump_messages(void)
{
	if (state_ptr)
	{
		MSG msg{};
		while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
	return true;
}

void* platform_allocate(u64 size, bool aligned)
{
	return malloc(size);
}
void platform_free(void* block, bool aligned)
{
	free(block);
}

void platform_zero_memory(void* block, u64 size)
{
	platform_set_memory(block,0,size);
}

void platform_copy_memory(void* dest, void* src, u64 size)
{
	memcpy(dest,src,size);
}

void platform_set_memory(void* block, i32 value, u64 size)
{
	memset(block,value,size);
}

void platform_console_write(const char* message, u8 color)
{
	HANDLE out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTextAttribute(out_handle,platform_level_lookup[color]);

	WriteConsoleA(out_handle, message, strlen(message), nullptr, nullptr);

	WriteConsoleA(out_handle, "\n", 1, nullptr, nullptr);
}

void platform_console_write_error(const char* message, u8 color)
{
	HANDLE err_handle = GetStdHandle(STD_ERROR_HANDLE);

	SetConsoleTextAttribute(err_handle,platform_level_lookup[color]);

	WriteConsoleA(err_handle,message,strlen(message),nullptr,nullptr);

	WriteConsoleA(err_handle, "\n", 1, nullptr, nullptr);
}

i32 get_number_of_logical_procressor(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	HLINFO("%d Processors Detected.",info.dwNumberOfProcessors);
	return info.dwNumberOfProcessors;
}

static LRESULT win32_msg_proc(HWND handle ,UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(handle,msg,wparam,lparam);
}
#endif
