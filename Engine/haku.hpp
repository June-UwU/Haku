#include "defines.hpp"
#include "core/logger.hpp"
#include "core/application.hpp"
extern i8 CreateGame();

int main(void)
{

	application_state app_state{};
	app_state.x 		= 100;		// x pos
	app_state.y		= 100;		// y pos
	app_state.height	= 720;		// height of window
	app_state.width		= 1080;		// width of window
	app_state.name		= "Haku test";	// app name

	application_initialize(&app_state);

	CreateGame();

	application_run(); // game loop
	
	return 0;
}
