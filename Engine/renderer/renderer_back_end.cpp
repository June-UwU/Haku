/*****************************************************************//**
 * \file   renderer_back_end.cpp
 * \brief  renderer backend implementations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "renderer_back_end.hpp"
#include "core/logger.hpp"
#include "DirectXMath.h"

using namespace DirectX;

// TODO  : notes there's not particular world space transformation , the per object transformation applied matrix makes it the world space transforms
// from that point it is further transformed into the image with view and projection space

// FIX ME : make sure that the screen size is part of the render packet and used approiately for the global transformation 

#include "backends/directx/directx.hpp"

i8 backend_initialize(renderer_backend* backend_ptr, i32 width, i32 height, f32 fov, backends api_type)
{
	i8 ret_code = H_OK;
	switch (api_type)
	{
	case HK_DIRECTX_12:
	{
		backend_ptr->api_type	 = api_type;
		backend_ptr->initialize	 = directx_initialize;
		backend_ptr->begin_frame = directx_begin_frame;
		backend_ptr->end_frame	 = directx_end_frame;
		backend_ptr->shutdown	 = directx_shutdown;
		backend_ptr->resize		 = directx_resize;
		//backend_ptr->update_global_transforms = directx_update_global_transforms;
		// TODO : XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.5f }, { 0.0f, 1.0f, 0.0f });
		backend_ptr->transforms.view_matrix = XMMatrixIdentity();
		backend_ptr->transforms.projection_matix = XMMatrixPerspectiveFovLH(fov, width / height, 0.1f, 100.0f);
		ret_code = backend_ptr->initialize(backend_ptr, &backend_ptr->transforms);
		if (H_OK != ret_code)
		{
			HLEMER("backend initialization failure");
			return H_FAIL;
		}
//		ret_code = backend_ptr->update_global_transforms(backend_ptr, &backend_ptr->transforms);
		break;
	}
	default:
	{
		HLEMER("Haku %s backend has not been implemented yet..", BACKEND_STRING);
		ret_code = H_FAIL;
	}
	}
	return ret_code;
}

void backend_shutdown(renderer_backend* backend_ptr)
{
	backend_ptr->shutdown(backend_ptr);
}
