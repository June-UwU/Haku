#include "include/haku.hpp"
#include "platform/window.hpp"
#include "logger.hpp"
#include "defines.hpp"

bool initialize_engine() {
	TRACE << "hello.. ^_^\n";
	auto window_initialised = initialize_windows(720, 1080);
	ASSERT(window_initialised, "failed to initialize windows.");
	return true;
}

void run_engine() {
	while (true == window_still_open()) {
		window_update();
	}

	terminate_window();
}

void terminate_engine() {
}
