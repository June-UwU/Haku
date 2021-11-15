#include "Haku.hpp"

int main(int argc, char** argv)
{
	char windowname[] = "Haku";
	uint32_t height = 720;
	uint32_t width = 1080;
	uint32_t maximize = false;
	Haku::Windows Window(height,width,maximize,windowname);
	Haku::Application app;
	app.SetMainWindow(Window);
	app.ProcessMessage();
	return 0;
}
