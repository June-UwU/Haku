#pragma once

#include "defines.hpp"
#include "directx_types.hpp"

#include <D3d12.h>

constexpr const u32 ALLOCATOR_COUNT = 6u;

// internal representation of queue
typedef enum
{
	HK_COMMAND_RENDER = 0x0,
	HK_COMMAND_COMPUTE,
	HK_COMMAND_COPY,

	HK_COMMAND_MAX

}queue_type;


// @breif		command queue initializations routine
// @return		: H_OK on sucess , else H_FAIL
i8 command_context_initialize(ID3D12Device* device);

// @breif		tear down routine for the underlying command routine
void command_context_shutdown(void);

// @breif c		excute any command 
void execute_command(queue_type type);