/*****************************************************************//**
 * \file   renderer_front_end.cpp
 * \brief  renderer front end implementation, this communicates with the backend
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "core/logger.hpp"
#include "memory/hmemory.hpp"
#include "renderer_front_end.hpp"
#include "renderer_back_end.hpp"
#include "DirectXMath.h"

using namespace DirectX;

/** backend pointer of the current backend */
static renderer_backend* backend;


void renderer_requirement(u64* memory_requirement)
{
	*memory_requirement = sizeof(renderer_backend);
}

i8 renderer_initialize(void* state, i32 width, i32 height, f32 fov, backends api_type)
{
	i8 ret_code = H_OK;

	backend = (renderer_backend*)state;

	ret_code = backend_initialize(backend, width, height, fov, api_type);

	return ret_code;
}

void renderer_shutdown(void)
{
	backend_shutdown(backend);
	backend = 0;
}

i8 renderer_resize(u16 height, u16 width, bool is_last)
{
	if (!is_last)
	{
		HLINFO("skipped backend reize event (height/width) : %d/%d", height, width);
		return H_NOERR;
	}
	if (backend && is_last)
	{
		HLINFO("backend reize event (height/width) : %d/%d", height, width);
		backend->resize(backend, height, width);
		return H_OK;
	}
	HLEMER("backend doesn't exist to handle reize event (height/width) : %d/%d", height, width);
	return H_FAIL;
}

// TODO : update delta_time of packet
i8 renderer_draw_frame(render_packet* packet)
{
	if (packet->height != backend->height || packet->width != backend->width || packet->fov != backend->fov)
	{
		if (packet->width > 0 && packet->height > 0)
		{
			backend->transforms.view_matrix = XMMatrixIdentity();
			backend->transforms.projection_matix = XMMatrixPerspectiveFovLH(packet->fov, packet->width / packet->height, 0.1f, 100.0f);
			// TODO : make sure this is properly checked
		//	backend->update_global_transforms(backend,&backend->transforms);
		}
	}

	i8 ret_code = backend->begin_frame(backend, packet->delta_time);

	if (H_OK == ret_code)
	{
		ret_code = backend->end_frame(backend, packet->delta_time);

		if (H_FAIL == ret_code)
		{
			HLEMER("end frame failed");
			return H_ERR;
		}
	}
	return ret_code;
}
