#pragma once

// haku renderer front end
#include "renderer_types.inl"

// @breif  	routine to initialize rendering subsystem
// @return 	: H_OK on success
i8 renderer_initialize(backends api_type);

// @breif	routine to shutdown rendering subsystem
void renderer_shutdown(void);

// @breif	routine to resize the renderer
// @param	: height to resize the renderer
// @param	: width to resize the renderer
// @return	: H_OK on sucess
i8 renderer_resize(u16 height, u16 width, bool is_last);

// @breif 	rendering routine
// @param	: rendering packet of metadata
// @return 	: H_OK on sucess
i8 renderer_draw_frame(render_packet* packet);
