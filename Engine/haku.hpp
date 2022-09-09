/*****************************************************************//**
 * \file   haku.hpp
 * \brief  haku include for client applications, this is a absolute nessary include
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "defines.hpp"
#include "core/logger.hpp"
#include "core/application.hpp"

/**
 * Client defined routine to create a game and register with the engine.
 * 
 * \return H_OK on success , else H_FAIL
 */
extern i8 CreateGame();

/**
 * Entry point.
 * 
 * \return 0 on sucess, negative result on error
 */
int main(void)
{
	i8				  ret_code = H_OK;
	application_state app_state{};
	app_state.x		 = 100;			// x pos
	app_state.y		 = 100;			// y pos
	app_state.height = 720;			// height of window
	app_state.width	 = 1080;		// width of window
	app_state.name	 = "Haku test"; // app name

	ret_code = application_initialize(&app_state);

	if (H_OK != ret_code)
	{
		HLEMER("application failed to initialize");
		return H_FAIL;
	}

	CreateGame();

	if (H_OK != ret_code)
	{
		HLEMER("game failed to initialize");
		return H_FAIL;
	}

	application_run(); // game loop

	return 0;
}
