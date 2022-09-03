#pragma once

#include "directx_types.inl"


// @breif 	routine to initialize  the swapchain for directx
// @param	: pointer to directx context
// @return 	: H_OK on sucess else H_FAIL
i8 swapchain_initialize(directx_context* context);

// @breif 	routine to shutdown a swapchain for directx
// @param	: pointer to swapchain
void swapchain_shutdown(directx_swapchain* swapchain);

// @breif	routine to set the render target up
// @param	: pointer to swapchain
// @param	: pointer to commandlist
// @return	: H_OK on sucess
i8 set_render_target(directx_swapchain* swapchain, directx_commandlist* commandlist);

// @breif	routine to set the present target up
// @param	: pointer to swapchain
// @param	: pointer to commandlist
// @return	: H_OK on sucess
i8 set_present_target(directx_swapchain* swapchain, directx_commandlist* commandlist);

// @breif	routine to set the render target up
// @param	: pointer to swapchain
// @param	: pointer to commandlist
// @param	: red value
// @param	: green value
// @param	: blue value
// @param	: alpha value
void clear_back_buffer(directx_commandlist* commandlist, directx_swapchain* swapchain, f32 r, f32 g, f32 b, f32 a);

// @breif	routine to present the frame
// @param	: pointer to swapchain
// @return	: H_OK on sucess
i8 present_frame(directx_swapchain* swapchain);

// @breif	routine to resize the frame
// @param	: pointer to directx context
// @param	: pointer to swapchain
// @param	: width of the swapchain
// @param	: height of the swapchain
// @return	: H_OK on sucess
i8 swapchain_resize(directx_context* context, directx_swapchain* swapchain, u16 width, u16 height);


// @breif	routine to clear depth stencil up
// @param	: pointer to swapchain
// @param	: pointer to commandlist
void clear_depth_stencil(directx_commandlist* commandlist, directx_swapchain* swapchain);

// @breif	routine to set the depth stencil up
// @param	: pointer to swapchain
// @param	: depth value
// @param	: stencil value
void set_depth_stencil(directx_swapchain* swapchain, f32 depth, u8 stencil);