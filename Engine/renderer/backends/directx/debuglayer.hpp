#pragma once

#include "defines.hpp"


#ifdef _DEBUG

#define ENABLE_DEBUG_LAYER() 	debug_layer_initialize()
#define DEBUG_LAYER_SHUTDOWN()	debug_layer_shutdown()

#else

#define	ENABLE_DEBUG_LAYER()
#define DEBUG_LAYER_SHUTDOWN()

#endif


// @breif	routine to enable and handle debug layers
// @return	: H_OK on sucess
i8 debug_layer_initialize(void);

// @breif 	routine to shutdown debug layer
void debug_layer_shutdown(void);
