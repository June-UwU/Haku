#pragma once

#include "renderer/renderer_back_end.hpp"

// @breif	directx initialization
// @param	: pointer to backend struct
// @return 	: H_OK on sucess
i8 directx_initialize(renderer_backend* backend_ptr);

// @breif	directx shutdown 
// @param	: pointer to backend struct
void directx_shutdown(renderer_backend* backend_ptr);

// @breif 	routine to begin frame
// @param	: backend pointer
// @param	: delta time 
// @return 	: H_OK on sucess
i8 directx_begin_frame(renderer_backend* backend_ptr, f64 delta_time);

// @breif 	routine to end frame
// @param	: backend pointer
// @param	: delta time 
// @return 	: H_OK on sucess, H_FAIL is  critical condition
i8 directx_end_frame(renderer_backend* backend_ptr,f64 delta_time);
