#include "Haku.hpp"
#include <iostream>

class Editor : public Haku::Application
{
};

int main(int argc, char** argv)
{
	char			 windowname[] = "Haku";
	uint32_t		 height		  = 720;
	uint32_t		 width		  = 1080;
	uint32_t		 maximize	  = false;
	Haku::Windows	 Window(height, width, maximize, windowname);
	Editor*			 app   = new Editor();
	app->SetMainWindow(Window);
	app->ProcessMessage();
	return 0;
}
