#pragma once

#include "defines.hpp"
#include "directx_types.INL"

// @breif		command queue initializations routine
// @return		: H_OK on sucess , else H_FAIL
i8 command_context_initialize(directx_context* context);

// @breif		tear down routine for the underlying command routine
void command_context_shutdown(directx_queue* context);

// @breif 		excute any command
// @param		: directx queue struct pointer
// @param		: commandlist struct pointer
void execute_command(directx_context* context, directx_commandlist* commandlist);

// @breif		prepare a commandlist for recording
// @param		: command list to be prepared
// @return 		: H_OK on sucess
i8 prepare_commandlist_record(directx_commandlist* commandlist);

// @breif		close and make nessary preparation for ending recording in a commandlist
// @param		: pointer to the current commandlist to end recording on
// @return		: H_OK on sucess
i8 end_commandlist_record(directx_commandlist* commanlist);

// @breif 		synchronize the command list and prepare for next frame
// @param		: queue pointer
// @param 		: commandlist to execute
// @return 		: H_OK on sucess
i8 next_frame_synchronization(directx_queue* queue, directx_commandlist* commandlist);

// @breif 		do a full gpu flush for clearing
// @param		: a queue to do the full gpu flush on
// @return		: H_OK on sucess
i8 full_gpu_flush(directx_queue* queue, queue_type type);
