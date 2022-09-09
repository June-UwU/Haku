/*****************************************************************//**
 * \file   swapchain.hpp
 * \brief  directx renderer swapchain system
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "directx_types.INL"

/** 
*  routine to initialize  the swapchain for directx
* \param context[in] pointer to directx context
* \return H_OK on sucess else H_FAIL
*/
i8 swapchain_initialize(directx_context* context);

/** 
*  routine to shutdown a swapchain for directx
* \param swapchain[in] pointer to swapchain
*/
void swapchain_shutdown(directx_swapchain* swapchain);

/** 
*  routine to set the render target up
* \param swapchain[[in] pointer to swapchain
* \param commandlist[in] pointer to commandlist
* \return H_OK on sucess
*/
i8 set_render_target(directx_swapchain* swapchain, directx_commandlist* commandlist);

/** 
*  routine to set the present target up
* \param swapchain[in] pointer to swapchain
* \param commandlist[in] pointer to commandlist
* \return H_OK on sucess
*/
i8 set_present_target(directx_swapchain* swapchain, directx_commandlist* commandlist);

/** 
*  routine to set the render target up
* \param commandlist[in] pointer to commandlist
* \param swapchain[in] pointer to swapchain
* \param r[in] red value
* \param g[in] green value
* \param b[in] blue value
* \param a[in] alpha value
*/
void clear_back_buffer(directx_commandlist* commandlist, directx_swapchain* swapchain, f32 r, f32 g, f32 b, f32 a);

/** 
* routine to present the frame.
* \param swapchain[in] pointer to swapchain
* \return H_OK on sucess
*/
i8 present_frame(directx_swapchain* swapchain);

/**  
* routine to resize the frame.
* \param context[in] pointer to directx context
* \param swapchain[in] pointer to swapchain
* \param width[in] width of the swapchain
* \param height[in] height of the swapchain
* \return H_OK on sucess
*/
i8 swapchain_resize(directx_context* context, directx_swapchain* swapchain, u16 width, u16 height);

/** 
* routine to clear depth stencil up
* 
* \param commandlist[in] pointer to directx_commandlist  that is recording
* \param swapchain[in] pointer to swapchain
*/
void clear_depth_stencil(directx_commandlist* commandlist, directx_swapchain* swapchain);
/** 
* routine to bind view port to the pipeline
* \param commandlist[in] pointer to swapchain
* \param swapchain[in] pointer to commandlist
*/
void bind_view_port(directx_commandlist* commandlist, directx_swapchain* swapchain);

/** 
* 	routine to bind view port to the pipeline
* \param commandlist[in] pointer to commandlist
* \param swapchain[in] pointer to swapchain
*/
void bind_scissor_rect(directx_commandlist* commandlist, directx_swapchain* swapchain);

/** 
*  routine to set the depth stencil up
* \param swapchain[in] pointer to swapchain
* \param left[in] left
* \param top[in] top
* \param right[in] right
* \param bottom[in] bottom
*/
void set_view_port(directx_swapchain* swapchain, i32 left, i32 top, i32 right, i32 bottom);

/** 
*  routine to set the depth stencil up
* \param swapchain[in] pointer to directx_swapchain
* \param width[in] width
* \param height[in] height
* \param topX[in] top left X coord
* \param topY[in] top left Y coord
*/
void set_view_port(directx_swapchain* swapchain, f32 width, f32 height, f32 topX, f32 topY);

/** 
* 	routine to set the depth stencil up
* \param swapchain[in] pointer to directx_swapchain
* \param depth[in] depth value
* \param stencil[in] stencil value
*/
void set_depth_stencil(directx_swapchain* swapchain, f32 depth, u8 stencil);
