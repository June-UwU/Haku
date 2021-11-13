#include "Haku.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	char windowname[] = "Haku";
	uint32_t height = 720;
	uint32_t width = 1080;
	uint32_t maximize = false;
	Haku::Window Window(height,width,maximize,windowname);
	Window.run();
	return 0;
}
