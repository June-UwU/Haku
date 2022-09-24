/*****************************************************************//**
 * \file   platform.hpp
 * \brief  platform specific function definitions for haku
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/** window properties. */
typedef struct p_prop
{
	/** window height */
	u32 height;

	/** window width */
	u32 width;
} p_prop;

// Haku platform subsystem

// TODO : don't export the platform code

/**
 * platform memory requirement.
 * 
 * \param memory_requirement pointer to be filled out
 */
void platform_requirement(u64* memory_requirement);

/**
 * platform initialzation code.
 * 
 * \param state generic structure that is defined per platform inside the transation unit
 * \param name application name
 * \param x position x for the window
 * \param y position y for the window
 * \param height height of the window
 * \param width width of the window
 * \return H_OK on sucess and H_FAIL on failure
 */
i8 platform_initialize(void* state, const char* name, u32 x, u32 y, u32 height, u32 width);

/**  platform specfic memory initialize.*/
i8 platform_memory_initialize();

/**  platform specfic memory shutdown */
void platform_memory_shutdown();

/**
 * platform clean up.
 */
void platform_shutdown();

/**
 * routine to pump messages.
 */
i8 platform_pump_messages(void);

/**
 * routine to zero out a block of memory.
 * 
 * \param block pointer to the block
 * \param size size of the block
 */
void platform_zero_memory(void* block, u64 size);

/** 
* routine to copy a block of memory. 
* 
* \param dest	 destination of the copy
* \param src		source of the copy
* \param size	 size in bytes to copy
*/
void platform_copy_memory(void* dest, void* src, u64 size);

/** 
* routine to set memory to a value.
*
* \param block	 destination of memset
* \param value	 value to set block
* \param size	 size of memory block to write
*/
void platform_set_memory(void* block, i8 value, u64 size);

/** 
* routine to write to console
*
* \param message message to write to console
* \param color   color enum
*/
void platform_console_write(const char* message, u8 color);

/**  
* routine to write to error console.
* 
* \param message message to write
* \param color   color enum
*/
void platform_console_write_error(const char* message, u8 color);

/**
 * routine to return the number of cores available.
 */
i32 get_number_of_logical_procressor(void);

/**  
* routine to return performance counter.
* 
* \return float timer
*/
f64 platform_time(void);

/**
* routine to set platform specfic data to api.
* 
* \param ptr pointer a pointer to fill the data
*/
void platform_data_for_render_api(void** ptr);
/**  
* \ routine to set platform generic data to api.
* 
* \param ptr	pointer to fill the data
* */
void get_platform_properties(p_prop* ptr);

/** routine to abort process */
void platform_abort(void);

/**
 * routine to exit with clean up. 
 * 
 * \param exit_code return code.
 */
void platform_exit(i32 exit_code);

/**
 * platform specfic routine to open a file.
 * 
 * \param file_name pointer to file name
 * \param mode file mode bit feild
 * \return pointer to the file that was opened. nullptr on fail
 */
void* platform_file_open(const char* file_name, const u64 mode);

/**
 * platform specfic routine to close a file.
 * 
 * \param file_ptr file pointer to the file
 */
void platform_file_close(void* file_ptr);

/**
 * platform specific file read function.
 * 
 * \param buffer pointer to the buffer to be filled
 * \param size size to read in bytes
 * \param file_ptr  file to read form
 * \return size that is read
 */
u64 platform_file_read(void* buffer, u64 size, const void* file_ptr);

/**
 * platform specific file write function.
 * 
 *\param file_ptr pointer to the buffer to be filled
 *\param size  size to write in bytes
 *\param buffer file to write from
 *\return size that is written
 */
u64 platform_file_write(void* file_ptr, u64 size, const void* buffer);


/**
 * platform specific routine to get the file size of a file.
 * 
 *\param file_ptr pointer to the file
 *\return	size of the file in bytes or zero
 */
u64 platform_get_file_size(void* file_ptr);

/**
* allocation platform  specfic.
* 
*\param size	  size of the pointer to return
*\param aligned bool for making the size align
*\return void pointer to the allocated the memory
*/
void* platform_allocate(u64 size, bool aligned);

/**
 * free for platform specfic allocate.
 * 
 *\param block   pointer to the allocated memory
 *\param aligned bool to indicate if its aligned
 */
void platform_free(void* block, bool aligned);

/**
 * routine for getting heap allocated mem size for supporting platform (win32 HeapSize()).
 * 
 *\param block pointer to the previously allocated block
 *\return	 size of the block
 */
u64 platform_alloc_size(void* block);

/**
 * platform specfic routine to make memory zeroed out.
 * 
 * \param block pointer to the block of memory to be zeroed out
 * \param size size of the memory block
 */
void platform_zero_memory(void* block, u64 size);

/**
 * platform specfic routine to copy memory.
 * 
 * \param dest destination to copy the memory
 * \param src pointer to memory to copy form
 * \param size size in bytes to copy
 */
void platform_copy_memory(void* dest, void* src, u64 size);

/**
 * platform specfic routine to set a block of memory to .
 * 
 * \param block pointer to the block of memory
 * \param value char/8 bit value to set memory to
 * \param size size of memory in bytes
 */
void platform_set_memory(void* block, i8 value, u64 size);




