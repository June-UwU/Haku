#include "platform/platform.hpp"

#ifdef HWIN32

#include "core/logger.hpp"
#include "core/event.hpp"
#include "core/input.hpp"
#include "memory/hmemory.hpp"

#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <windowsx.h>
#include <sysinfoapi.h>

static const char* UNRESOLVED_WIN_ERROR = "Api message failure";

typedef struct platform_state
{
	HINSTANCE hinstance;
	HWND 	  hwnd;
}platform_state;

static platform_state* 	state_ptr; // platform dependant state

static u32          p_height;
static u32          p_width;
static HANDLE 		private_heap; 	  // windows specfic heaps
static DWORD  		heap_flags = HEAP_GENERATE_EXCEPTIONS;  // generate exception if heap_allocation fails
static f64		start_time;

static constexpr WORD platform_level_lookup[LOG_LVL_COUNT]
{
	BACKGROUND_RED | BACKGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_GREEN
};


static LRESULT win32_msg_proc(HWND handle ,UINT msg, WPARAM wparam, LPARAM lparam);

// Internal function to create error strings
void win32_get_error_string(DWORD error_code)
{
	char* ret_ptr = const_cast<char*>(UNRESOLVED_WIN_ERROR);
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,error_code,
			MAKELANGID(LANG_NEUTRAL , SUBLANG_DEFAULT),ret_ptr,0,NULL);

	// cleaner and better implementation
	if ( UNRESOLVED_WIN_ERROR == ret_ptr)
	{
		HLEMER(ret_ptr);
	}	
	else
	{
		HLEMER(ret_ptr);
		LocalFree(ret_ptr);
	}
	
}

i8 platform_initialize(void* state, const char* name, u32 x, u32 y, u32 height, u32  width)
{	
	p_height        = height;
	p_width         = width;

	state_ptr 		= (platform_state*)hmemory_alloc(sizeof(platform_state),MEM_TAG_PLATFORM);

	state 			= state_ptr;

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

	LARGE_INTEGER timer;
	bool ret_code = QueryPerformanceCounter(&timer);

	if( false == ret_code)
	{
		HLCRIT("Timer  failed to initialize");
	}

	start_time = (f64)timer.QuadPart/1000.0f;

	return H_OK;
}

void platform_shutdown()
{
	hmemory_free(state_ptr,MEM_TAG_PLATFORM);
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

i8 platform_memory_initialize()
{
	// heap initial size is made to be zero and commits one page
	// heap max is set to zero which limits the heap allcation based on available memory
	// the largest allocation that can be made from the heap is 
	// 	32 bit system : 512  KB
	// 	64 bit system : 1024 KB
	private_heap = HeapCreate(heap_flags,0,0);
	
	if(!private_heap)
	{
		DWORD error = GetLastError();
		win32_get_error_string(error);
		HLEMER("Windows private heap allocation failed");
		return H_FAIL;
	}
	return H_OK;
}

void platform_memory_shutdown()
{
	HeapDestroy(private_heap);
}

void* platform_allocate(u64 size, bool aligned)
{
	void* ret_ptr = HeapAlloc(private_heap,heap_flags,size);

	if(nullptr == ret_ptr)
	{
		HLEMER("Heap allocation failure");
		DWORD error = GetLastError();
		win32_get_error_string(error);
		exit(H_FAIL); // aborting application, generated crash
	}

	return ret_ptr;
}

void platform_free(void* block, bool aligned)
{
	HeapFree(private_heap,heap_flags,block);
}

u64   platform_alloc_size(void* block)
{
	return HeapSize(private_heap,0,block);
}

void platform_zero_memory(void* block, u64 size)
{
	ZeroMemory(block,size);
}

void platform_copy_memory(void* dest, void* src, u64 size)
{
	CopyMemory(dest,src,size);
}

void platform_set_memory(void* block, i8 value, u64 size)
{
	FillMemory(block,size,value);
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
	switch(msg)
	{
		case WM_KEYDOWN:
			{
				switch(wparam)
				{
					case VK_MBUTTON:
						{
							input_process_button(BUTTON_MIDDLE, true);
							break;
						}
					case VK_LBUTTON:
						{
							input_process_button(BUTTON_LEFT, true);
							break;
						}
					case VK_RBUTTON:
						{
							input_process_button(BUTTON_RIGHT, true);
							break;
						}
					default:
						input_process_key((keys)wparam, false);
				}
				return 0;
			}
		case WM_KEYUP:
			{
				switch(wparam)
				{
					case VK_MBUTTON:
						{
							input_process_button(BUTTON_MIDDLE,false);
							break;
						}
					case VK_LBUTTON:
						{
							input_process_button(BUTTON_LEFT, false);
							break;
						}
					case VK_RBUTTON:
						{
							input_process_button(BUTTON_RIGHT, false);
							break;
						}
					default:						
							input_process_key((keys)wparam,false);
						
				}
				return 0;
			}
		case WM_MOUSEMOVE:
			{
				input_process_mouse_move(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
				return 0;
			}
		case WM_MOUSEWHEEL:
			{
				input_process_mouse_wheel((i8)GET_WHEEL_DELTA_WPARAM(wparam));
				return 0;
			}
		case WM_CLOSE:
			{
				event_fire(HK_EXIT,NULL, 0);
				return 0;
			}
		case WM_SIZE:
			{
				u16 p_width 	= LOWORD(lparam);
				u16 p_height 	= HIWORD(lparam);
				i64 context = PACK_64BIT_VALS(0, 0, p_height, p_width);
			    	switch (wparam)
				{
					case SIZE_MINIMIZED:
						{
							event_fire(HK_MINIMIZE,nullptr,context);
						}
					default:
						{
							event_fire(HK_SIZE,nullptr,context);
						}
				}
			}
	}
	return DefWindowProc(handle, msg, wparam, lparam);
}

f64 platform_time(void)
{
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);

	f64 ret_val	= (f64)tick.QuadPart/1000.0f;

	return  ret_val - start_time;
}
void platform_data_for_render_api(void** ptr)
{
	*ptr = (void*)&state_ptr->hwnd;
}
void get_platform_properties(p_prop* ptr)
{
	ptr->height = p_height;
	ptr->width  = p_width;
}

void platform_abort(void)
{
	ExitProcess(H_FAIL);
}

void platform_exit(i32 exit_code)
{
	bool api_ret = PostMessageA(state_ptr->hwnd, WM_CLOSE, 0, 0);
	if (false == api_ret)
	{
		HLCRIT("exit has been call failed ");
	}
}


#endif