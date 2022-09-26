/*****************************************************************//**
 * \file   application.hpp
 * \brief  haku applications
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/** applications state */
HAPI typedef struct application_state
{
	/** window x position */
	i32			x;		

	/** window y position */
	i32			y;		

	/** height of window */
	i32			height; 

	/** width of window */
	i32			width;	

	/** name of application */
	const char* name;	
} application_state;

/**
 *routine to application entry point .
 * 
 * \param app_state pointer to an application state
 * \return H_OK on sucess and H_FAIL on failure
 */
HAPI i8 application_initialize(application_state* app_state);

/**
 * routine to run application (game loop) shutdown is handled internally.
 */
HAPI void application_run(void);




