#pragma once

#include "directx_types.inl"


// @breif 	routine to initialize  the swapchain for directx
// @param	: pointer to directx context
// @return 	: H_OK on sucess else H_FAIL
i8 swapchain_initialize(directx_context* context);

// @breif 	routine to shutdown a swapchain for directx
void swapchain_shutdown(directx_swapchain* swapchain);


i8 set_render_target(directx_swapchain* swapchain, directx_commandlist* commandlist);

i8 set_present_target(directx_swapchain* swapchain, directx_commandlist* commandlist);

void clear_back_buffer(directx_commandlist* commandlist, directx_swapchain* swapchain, f32 r, f32 g, f32 b, f32 a);

i8 present_frame(directx_swapchain* swapchain);