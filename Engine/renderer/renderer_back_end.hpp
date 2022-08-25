#pragma once


#include "renderer_types.inl"

// @breif	routine to initialize backend 
// @param	: pointer to the backend
// @param	: api type
// @return	: H_OK on sucess
i8 backend_initialize(renderer_backend* backend_ptr,backends api_type);

// @breif	routine to shutdown renderer backend
// @param	: pointer to backend struct
void backend_shutdown(renderer_backend* backend_ptr);
