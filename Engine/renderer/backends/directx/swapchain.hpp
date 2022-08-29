#pragma once

#include "directx_types.inl"


// @breif 	routine to initialize  the swapchain for directx
// @param	: pointer to directx context
// @return 	: H_OK on sucess else H_FAIL
i8 swapchain_initialize(directx_context* context);

// @breif 	routine to shutdown a swapchain for directx
void swapchain_shutdown(directx_swapchain* swapchain);


