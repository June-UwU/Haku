#include "Haku.hpp"
#include <iostream>
namespace Haku
{
class TestLayer : public Layer
{
public:
	TestLayer()
		: Layer("Test Layer")
	{
	}
	void OnAttach() override
	{
		std::cout << "On Attach called"
				  << "\n";
	}
	bool OnEvent(Event& event) override
	{
		std::cout << "OnEvent Testlayer"
				  << "\n";
		return false;
	}
};

}; // namespace Haku
class Editor : public Haku::Application
{
};

int main(int argc, char** argv)
{
	char		  windowname[] = "Haku";
	uint32_t	  height	   = 720;
	uint32_t	  width		   = 1080;
	uint32_t	  maximize	   = false;
	Haku::Windows Window(height, width, maximize, windowname);
	Haku::TestLayer*	  layer = new Haku::TestLayer();
	Editor*		  app = new Editor();
	app->AddLayer(layer);
	app->SetMainWindow(Window);
	app->ProcessMessage();
	return 0;
}
