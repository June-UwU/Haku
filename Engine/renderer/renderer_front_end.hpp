/*****************************************************************//**
 * \file   renderer_front_end.hpp
 * \brief  renderer front-end that acts as a general interface for the entire engine
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "renderer_types.inl"


/**
 * renderer memory requirement.
 * \param  memory_requirement[out] pointer to be filled out
 */
void renderer_requirement(u64* memory_requirement);

/**
 * routine to initialize rendering subsystem.
 * 
 * \param state[in] pointer to allocated memory
 * \param api_type[in] backend type 
 * \return	 H_OK on success
 */
i8 renderer_initialize(void* state, backends api_type);

/**
 * routine to shutdown the render-frontend.
 */
void renderer_shutdown(void);

/**
 * routine to resize the renderer.
 * 
 * \param height[in] height to resize the renderer
 * \param width[in] width to resize the renderer
 * \param is_last[in] boolean to notify if this is the last message from event subsystem
 * \return H_OK on sucess
 */
i8 renderer_resize(u16 height, u16 width, bool is_last);

/**
 * rendering routine, called once per frame.
 * 
 * \param packet[in] rendering packet of metadata
 * \return H_OK on sucess
 */
i8 renderer_draw_frame(render_packet* packet);




