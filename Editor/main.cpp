#include "Haku.hpp"
#include <iostream>

class Editor : public Haku::Application
{
public:
	void ClientUpdate() override
	{
		Haku::UI::Begin();
		ui.Render();
		m_Menu.Render();
		m_Left.Render();
		Haku::UI::Render();
		Haku::UI::EndFrame();
	}
	void ClientInits() override { Haku::UI::InitUI(); }
	void ClientCleanUp() override { Haku::UI::CleanUp(); }

private:
	Haku::UI::DemoWindow ui;
	Haku::UI::Menubar	 m_Menu;
	Haku::UI::LeftMenu	 m_Left;
};

int main(int argc, char** argv)
{
	try
	{
		char					windowname[] = "Haku";
		uint32_t				height		 = 720;
		uint32_t				width		 = 1080;
		uint32_t				maximize	 = false;
		Haku::Windows			Window(height, width, maximize, windowname);
		std::unique_ptr<Editor> app = std::make_unique<Editor>();
		app->SetMainWindow(Window);
		app->ClientInits();
		app->ProcessMessage();
		return 0;
	}
	catch (Haku::Errors::Error err)
	{
		MessageBoxA(nullptr, err.what(), "Error!", 0);
	}
	catch (Haku::Errors::WinError err)
	{
		MessageBoxA(nullptr, err.what(), "Error!", 0);
	}
}
