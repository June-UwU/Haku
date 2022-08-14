#pragma once

#include "defines.hpp"

// Haku platform subsystem

	//TODO : don't export the platform code

	// @breif	 platform initialzation
	// @param state : generic structure that is defined per platform inside the transation unit
	// @param name  : application name
	// @param x 	: position x for the window
	// @param y	: position y for the window 
	// @param height: height of the window
	// @param width	: width of the window
	// @return 	: true on sucess and false on failure
	HAPI i32 platform_initialize( void* state, const char* name, u32 x, u32 y, u32 height, u32  width);
	
	//@breif  	 platform clean up
	//@param state 	: platform state 
	HAPI void platform_shutdown();

	//@brief 	 routine to pump messages
	HAPI i8 platform_pump_messages(void);

	// @breif	 allocation platform  specfic
	// @param size	 : size of the pointer to return
	// @param aligned: bool for making the size align
	// @return 	 : void pointer to the allocated the memory
	void* platform_allocate(u64 size, bool aligned);

	// @breif	 free for platform specfic allocate
	// @param block  : pointer to the allocated memory
	// @param aligned: bool to indicate if its aligned
	void platform_free(void* block, bool aligned);

	// @breif	 routine to zero out a block of memory
	// @param block	: pointer to the block
	// @param size	: size of the block
	void platform_zero_memory(void* block, u64 size);

	// @breif 	 routine to copy a block of memory
	// @param dest	: destination of the copy 
	// @param src	: source of the copy
	// @param size	: size in bytes to copy 
	void platform_copy_memory(void* dest, void* src, u64 size);

	// @breif	 routine to set memory to a value
	// @param block	: destination of memset
	// @param value	: value to set block
	// @param size	: size of memory block to write
	void platform_set_memory(void* block, i32 value, u64 size);

	// @breif 	 routine to write to console
	// @param message: message to write to console
	// @param color  : color enum  
	void platform_console_write(const char* message, u8 color);

	// @breif 	routine to write to error console
	//  @param message: message to write
	//  @param color  : color enum 
	void platform_console_write_error(const char* message, u8 color);

	// @breif 	routine to return the number of cores available
	i32 get_number_of_logical_procressor(void);
