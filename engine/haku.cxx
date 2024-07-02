#include "include/haku.hpp"
#include "defines.hpp"
#include "graphics/graphics.hpp"
#include "logger.hpp"
#include "platform/window.hpp"

bool initialize_engine() {
	TRACE << "hello.. ^_^\n";
	bool window_initialised = initialize_windows(720, 1080);
	ASSERT(window_initialised, "failed to initialize windows.");

	bool gpu_initialization = initialize_graphics();
	ASSERT(gpu_initialization, "failed to initialize graphics stack.");

	return true;
}

void run_engine() {
	while (true == window_still_open()) {
		window_update();
		u32 image_index = accquire_image();
		// record command
		submit(image_index);
		present(image_index);
	}
}

void terminate_engine() {
	terminate_graphics();
	terminate_window();
}
