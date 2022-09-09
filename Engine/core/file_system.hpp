#pragma once

#include "defines.hpp"

typedef enum
{
	READ			= BIT(0),
	WRITE			= BIT(1),
	APPEND			= BIT(2),
	READ_EXTENDED	= BIT(3),
	WRITE_EXTENDED	= BIT(4),
	APPEND_EXTENDED = BIT(5),
} file_mode;

typedef struct file
{
	// file mode that it is opened with
	file_mode mode;

	// pointer to the opened file
	void* file_ptr;
} file;

// @breif 	routine to open a file
// @param	: file name to open or create
// @param 	: file mode to use open
// @return 	: file pointer
HAPI file* file_open(const char* filename, const file_mode mode);

// @breif 	routine to close an open file
// @param	: pointer to file that was opened
// @return	: haku return code , H_OK
HAPI i8 file_close(file* file_ptr);

// @breif	routine to read from a file(THIS ROUTINE ALLOCATES MEMORY)
// @param 	: destination buffer
// @param 	: size of the objects to read
// @param	: count of the objects to read
// @return 	: number of bytes read
HAPI u64 file_read(void* buffer, u64 size, u64 count, const file* file_ptr);

// @breif 	routine to write to a file
// @param	: destination file ptr
// @param	: size of the objects to write
// @param	: count of the objects to read
// @param	: pointer to the data that needs to written
// @return 	: number of bytes written
HAPI u64 file_write(file* file_ptr, u64 size, u64 count, const void* buffer);

// @breif	routine to read an entire file
// @param	: destinaton pointer
// @param	: pointer to the file
// @return	: number of bytes read
HAPI u64 file_read_entire(void* buffer, file* file_ptr);

// @breif	routine to get the file permisson character string
// @param	: file mode
// @return	: character string
constexpr const char* file_perm(const u64 mode)
{
	switch (mode)
	{
	case READ:
	{
		return "READ";
	}
	case WRITE:
	{
		return "WRITE";
	}
	case APPEND:
	{
		return "APPEND";
	}
	case READ_EXTENDED:
	{
		return "READ_EXTENDED";
	}
	case WRITE_EXTENDED:
	{
		return "WRITE_EXTENDED";
	}
	case APPEND_EXTENDED:
	{
		return "APPEND_EXTENDED";
	}
	}
	return "UNKNOWN MODE";
}
