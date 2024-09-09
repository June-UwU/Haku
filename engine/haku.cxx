#include "include/haku.hpp"
#include "defines.hpp"
#include "logger.hpp"
#include "platform/window.hpp"
#include "renderer/renderer.hpp"

bool initialize_engine() {
	TRACE << "hello.. ^_^\n";
	bool window_initialised = initialize_windows(720, 1080);
	ASSERT(window_initialised, "failed to initialize windows.");

	bool renderer_initialized = initialize_renderer(1080,720);
	ASSERT(renderer_initialized, "failed to initialize renderer.");

	return true;
}

void run_engine() {
	u64 frame = 0;
	while (true == window_still_open()) {
		window_update();
		draw();
	}
	terminate_engine();
}

void terminate_engine() {
	shutdown_renderer();
	terminate_window();
}
