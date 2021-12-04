#pragma once
#include "Events.hpp"
#include "../macros.hpp"
#include "../Platform/Windows/MainWindow.hpp"
#include "../Platform/DirectX12/D3D12Renderer.hpp"
#include "UILayer.hpp"

namespace Haku
{
class HAKU_API Application
{
public:
	Application();
	void SetMainWindow(Windows& window) noexcept;

	/*might get redefined to return exit code*/
	void					ProcessMessage();
	void					OnEvent(Event Event);
	static Application*		Get() { return s_Instance; }
	Windows*				GetWindow() { return m_Window; }
	Renderer::DX12Renderer* GetRenderer() { return &m_Renderer; }

private:
	void Onclose(Event& Close);
	/*stud event to handle the unmapped event for the time being*/
	bool return_true(Event& event)
	{
		HAKU_LOG_WARN(__FUNCTION__);
		return true;
	}

private:
	EventDispatcher		   Dispatcher;
	bool				   m_Running = true;
	Windows*			   m_Window	 = nullptr;
	Renderer::DX12Renderer m_Renderer;

	UILayer layer;

private:
	static Application* s_Instance;
};
} // namespace Haku
