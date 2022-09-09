/*****************************************************************//**
 * \file   renderer_back_end.cpp
 * \brief  renderer backend implementations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "renderer_back_end.hpp"
#include "core/logger.hpp"

#include "backends/directx/directx.hpp"

i8 backend_initialize(renderer_backend* backend_ptr, backends api_type)
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
		ret_code				 = backend_ptr->initialize(backend_ptr);

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
