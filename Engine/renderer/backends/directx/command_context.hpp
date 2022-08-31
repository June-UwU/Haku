#pragma once

#include "defines.hpp"
#include "directx_types.inl"

// @breif		command queue initializations routine
// @return		: H_OK on sucess , else H_FAIL
i8 command_context_initialize(directx_context* context);

// @breif		tear down routine for the underlying command routine
void command_context_shutdown(directx_queue* context);

// @breif c		excute any command 
void execute_command(directx_context* context,directx_commandlist* commandlist);

i8 prepare_commandlist_record(directx_commandlist* commandlist);

i8 end_commandlist_record(directx_commandlist* commanlist);