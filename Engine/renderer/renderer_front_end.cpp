
#include "core/logger.hpp"
#include "core/hmemory.hpp"
#include "renderer_front_end.hpp"
#include "renderer_back_end.hpp"


// back end pointer
static renderer_backend* backend;



i8 renderer_initialize(backends api_type)
{
	i8 ret_code	= H_OK;
	
	backend		= (renderer_backend*)hmemory_alloc(sizeof(renderer_backend),MEM_TAG_RENDERER);

	ret_code	= backend_initialize(backend,api_type);
	
	return ret_code;
}

void renderer_shutdown(void)
{
	backend_shutdown(backend);
	hmemory_free(backend,MEM_TAG_RENDERER);
}

// TODO : update delta_time of packet
i8 renderer_draw_frame(render_packet* packet)
{
	i8 ret_code = backend->begin_frame(backend, packet->delta_time);

	if(H_OK == ret_code)
	{
		ret_code	= backend->end_frame(backend,packet->delta_time);

		if( H_FAIL == ret_code)
		{
			HLEMER("end frame failed");
			return H_ERR;
		}	
	}
	return ret_code;
}
