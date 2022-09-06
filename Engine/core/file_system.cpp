#include "logger.hpp"
#include "file_system.hpp"
#include "memory/hmemory.hpp"
#include "platform/platform.hpp"


HAPI file* file_open(const char* filename, const file_mode mode)
{
	file* ret_ptr = (file*)hmemory_alloc(sizeof(file), MEM_TAG_CORE);
	ret_ptr->mode = mode;
	ret_ptr->file_ptr = platform_file_open(filename, mode);

	if (nullptr == ret_ptr->file_ptr)
	{
		HLCRIT("file failed to open !");
		hmemory_free(ret_ptr, MEM_TAG_CORE);
		return nullptr;
	}

	return ret_ptr;
}

HAPI i8 file_close(file* file_ptr)
{
	platform_file_close(file_ptr->file_ptr);
	hmemory_free(file_ptr, MEM_TAG_CORE);
	return H_OK;
}

HAPI u64 file_read(void* buffer, u64 size, u64 count, const file* file_ptr)
{
	u64 ret_val = 0;

	buffer = hmemory_alloc(size * count, MEM_TAG_CORE);

	ret_val = platform_file_read(buffer, size * count, file_ptr->file_ptr);

	return ret_val;
}

HAPI u64 file_write(file* file_ptr, u64 size, u64 count, const void* buffer)
{
	u64 ret_val = 0;

	ret_val = platform_file_write(file_ptr->file_ptr, size * count, buffer);

	return ret_val;
}
