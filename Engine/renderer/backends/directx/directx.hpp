/*****************************************************************//**
 * \file   directx.hpp
 * \brief  directx backend implementations header
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

// NOTE : most subsystems have what is defined as fail handlers and fail code , these are used for potential fail
// handles and
//		  early development used gotos and they required c99 style initialization which was then replaced with the fail
//codes 		  and handlers these are indended to be internal to the subsystems and is unique for each subsystem

#include "renderer/renderer_back_end.hpp"

/**
 * routine for directx initialization.
 * 
 * \param backend_ptr pointer to backend struct
 * \return H_OK on sucess
 */
i8 directx_initialize(renderer_backend* backend_ptr,void* data);

/**
 * routine for directx shutdown.
 * 
 * \param backend_ptr pointer to backend struct
 */
void directx_shutdown(renderer_backend* backend_ptr);

/**
 * routine to begin frame.
 * 
 * \param backend_ptr pointer to backend 
 * \param delta_time delta time for the last call
 * \return H_OK on sucess else H_FAIL
 */
i8 directx_begin_frame(renderer_backend* backend_ptr, f64 delta_time);

/**
 * routine to end frame.
 * 
 * \param backend_ptr pointer to the backend 
 * \param delta_time delta time from the last call
 * \return H_OK on sucess, else H_FAIL is critical condition
 */
i8 directx_end_frame(renderer_backend* backend_ptr, f64 delta_time);

/**
 * function to resize the directx backend.
 *
 * \param backend_ptr pointer to the backend
 * \param height height to resize to
 * \param width width to resize to
 * \return H_OK on sucess 
 */
i8 directx_resize(renderer_backend* backend_ptr, u16 height, u16 width);




