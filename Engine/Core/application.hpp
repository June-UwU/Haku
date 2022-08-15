#include "defines.hpp"


HAPI typedef struct application_state
{
	i32 x;			// window x position
	i32 y;			// window y position
	i32 height;		// height of window
	i32 width;		// width of window
	const char* name;	// name of application
}application_state;

// @breif 	routine to application entry point
HAPI i8 application_initialize(application_state* app_state);

// @brief 	routine to run application (game loop) shutdown is handled internally
HAPI void application_run(void);

