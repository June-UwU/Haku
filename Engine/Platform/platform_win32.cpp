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

static platform_state* state_ptr; // platform dependant state

static HANDLE private_heap; 	  // windows specfic heaps
static DWORD  heap_flags = HEAP_GENERATE_EXCEPTIONS;  // generate exception if heap_allocation fails

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
char* win32_get_error_string(DWORD error_code)
{
	char* ret_ptr = nullptr;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,error_code,
			MAKELANGID(LANG_NEUTRAL , SUBLANG_DEFAULT),ret_ptr,0,NULL);

	// there is a small mem leak here from LocalAlloc()
	// LocalFree() if possible
	if(nullptr == ret_ptr)
	{
		ret_ptr = const_cast<char*>("Unknown Error");
	}

	return ret_ptr;
}


	// TODO : make use of state
i8 platform_initialize(void* state, const char* name, u32 x, u32 y, u32 height, u32  width)
{	
	// TODO : make this haku internal memory allocation
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
		char* message = win32_get_error_string(error);
		HLEMER("Windows private heap allocation failed");
		HLEMER(message);
		LocalFree(message); // This frees the buffer
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
		char* message = win32_get_error_string(error);
		HLEMER(message);
		LocalFree(message);
		exit(H_FAIL); // aborting application, generated crash
	}

	return ret_ptr;
}

void platform_free(void* block, bool aligned)
{
	HeapFree(private_heap,heap_flags,block);
}

u8   platform_alloc_size(void* block)
{
	return HeapSize(private_heap,heap_flags,block);
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
