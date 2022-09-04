#include "hstddef.hpp"
#include "core/logger.hpp"
#include "platform/platform.hpp"

void habort(void)
{
	HLCRIT("abort has been called ");
	platform_abort();
}

void hexit(i32 exit_code)
{
	HLWARN("exit has been called ");
	platform_exit(exit_code);
}
