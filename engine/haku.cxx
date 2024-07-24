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
	u64 frame = 0;
	while (true == window_still_open()) {
		window_update();
		u32 image = accquire_image(frame);
		record_image(frame, image);
		submit_image(frame, image);
		present_image(frame, image);
	}

	wait_on_gpu_till_idle();
}

void terminate_engine() {
	terminate_graphics();
	terminate_window();
}
