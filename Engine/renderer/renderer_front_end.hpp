#pragma once

// haku renderer front end
#include "renderer_types.inl"

// @breif  	routine to initialize rendering subsystem
// @return 	: H_OK on success
i8 renderer_initialize(backends api_type);

// @breif	routine to shutdown rendering subsystem
void renderer_shutdown(void);

// @breif 	rendering routine
// @param	: rendering packet of metadata
// @return 	: H_OK on sucess
i8 renderer_draw_frame(render_packet* packet);
