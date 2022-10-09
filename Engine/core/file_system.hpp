/*****************************************************************//**
 * \file   file_system.hpp
 * \brief  haku file operations header file
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

// TODO :  this documentation needs to be fact checked
/** bit feild to pass for the file permission */
typedef enum file_mode
{
	/** read permission */
	READ			= BIT(0),

	/** write permission */
	WRITE			= BIT(1),

	/** append permission */
	APPEND			= BIT(2),

	/** read and append */
	READ_EXTENDED	= BIT(3),

	/** write and append */
	WRITE_EXTENDED	= BIT(4),

	/** write and append */
	APPEND_EXTENDED = BIT(5),
} file_mode;

/** struct that represent a opened file */
typedef struct file
{
	/**  file mode that it is opened with */
	file_mode mode;

	/** pointer to the opened file */
	void* file_ptr;
} file;

/**
 * routine to open a file.
 * 
 * \param filename file name to open or create
 * \param mode file mode to use open
 * \return file pointer
 */
HAPI file* file_open(const char* filename, const file_mode mode);

/**
 * routine to close an open file.
 * 
 * \param file_ptr pointer to file that was opened
 * \return H_OK on sucess and H_FAIL on fail
 */
HAPI i8 file_close(file* file_ptr);

/**
 * routine to read from a file(THIS ROUTINE ALLOCATES MEMORY).
 * 
 * \param buffer destination buffer
 * \param size size of the objects to read
 * \param count count of the objects to read
 * \param file_ptr file pointer that 
 * \return number of bytes read
 */
HAPI u64 file_read(void* buffer, u64 size, u64 count, const file* file_ptr);

/**
 * routine to write to a file.
 * 
 * \param file_ptr destination file ptr
 * \param size size of the objects to write
 * \param count count of the objects to read
 * \param buffer pointer to the data that needs to written
 * \return number of bytes written
 */
HAPI u64 file_write(file* file_ptr, u64 size, u64 count, const void* buffer);

/**
 * routine to read an entire file.
 * 
 * \param buffer destinaton pointer
 * \param file_ptr pointer to the file
 * \return number of bytes read
 */
HAPI u64 file_read_entire(void* buffer, file* file_ptr);

/**
 * routine to get the file permisson character string.
 * 
 * \param mode file mode
 * \return character string
 */
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
