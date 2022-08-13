#include "platform.hpp"
#define HWIN32
#ifdef HWIN32
#include <Windows.h>
#include <string.h>
#include <Core\logger.hpp>
#include <sysinfoapi.h>

static constexpr WORD platform_level_lookup[LOG_LVL_COUNT]
{
	BACKGROUND_RED | BACKGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_GREEN
};


	// TODO : next in line to inplement
i32 platform_initialize(void* state, const char* name, u32 x, u32 y, u32 height, u32  width)
{
	return 0;
}

void platform_shutdown(void* state)
{

}

void* platform_allocate(u64 size, bool aligned)
{
	return nullptr;
}
void platform_free(void* block, bool aligned)
{

}

void platform_zero_memory(void* block, u64 size)
{

}

void platform_copy_memory(void* dest, void* src, u64 size)
{

}

void platform_set_memory(void* block, i32 value, u64 size)
{

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
#endif
