/*****************************************************************//**
 * \file   renderer_back_end.hpp
 * \brief  renderer backend routines that are needed for the implementation of different backends
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "renderer_types.inl"


/** 
* routine to initialize backend.
* 
* \param backend_ptr[in,out] pointer to the backend
* \param api_type[in] api type
* \return H_OK on sucess
*/
i8 backend_initialize(renderer_backend* backend_ptr, i32 width, i32 height, f32 fov, backends api_type);
/** 
* routine to shutdown renderer backend.
* \param backend_ptr[in] pointer to backend struct
*/
void backend_shutdown(renderer_backend* backend_ptr);
