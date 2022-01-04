#include "Haku.hpp"
#include <iostream>

class UI : Haku::UILayer
{
public:
	void Render() override
	{
		Haku::UI::DX12_newframe();
		Haku::UI::win32_newframe();
		Haku::UI::generic_new_frame();
		Haku::UI::DemoWindow();
		Haku::UI::render();
	};

private:
};

class Editor : public Haku::Application
{
public:
	void ClientUpdate() override { ui.Render(); }
	void ClientInits() override
	{
		Haku::UI::CreateContext();
		auto* app		 = Haku::Application::Get();
		auto* hwnd		 = app->GetWindow()->GetHandle();
		auto* device	 = app->GetRenderer()->GetDevice();
		auto* descriptor = app->GetRenderer()->GetDesciptor();

		Haku::UI::win32_Init(hwnd);
		Haku::UI::DX12_Init(
			device,
			2,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			descriptor,
			descriptor->GetCPUDescriptorHandleForHeapStart(),
			descriptor->GetGPUDescriptorHandleForHeapStart());
		Haku::UI::DarkMode();
	}
	void ClientCleanUp() override
	{
		Haku::UI::DX12_shutdown();
		Haku::UI::win32_shutdown();
		Haku::UI::DestroyContext();
	}

private:
	UI ui;
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
