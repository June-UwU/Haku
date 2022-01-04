#pragma once
#include "Events.hpp"
#include "UILayer.hpp"
#include "../macros.hpp"
#include "../Platform/Windows/MainWindow.hpp"
#include "../Platform/DirectX12/D3D12Renderer.hpp"

namespace Haku
{
class HAKU_API Application
{
public:
	Application();
	virtual void			ClientInits(){};
	virtual void			ClientUpdate(){};
	void					ProcessMessage();
	virtual void			ClientCleanUp(){};
	void					OnEvent(Event Event);
	static Application*		Get() { return s_Instance; }
	Windows*				GetWindow() { return m_Window; }
	Renderer::DX12Renderer* GetRenderer() { return &m_Renderer; }
	void					SetMainWindow(Windows& window) noexcept;

private:
	void Onclose(Event& Event);
	void OnResize(Event& Event);
	void OnMinimize(Event& Event);
	/*stud event to handle the unmapped event for the time being*/
	bool return_true(Event& event)
	{
		HAKU_LOG_WARN(__FUNCTION__);
		return true;
	}

private:
	EventDispatcher		   Dispatcher;
	Renderer::DX12Renderer m_Renderer;
	bool				   m_Running = true;
	Windows*			   m_Window	 = nullptr;

private:
	static Application* s_Instance;
};
} // namespace Haku
