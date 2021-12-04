#include "UILayer.hpp"
#include "Application.hpp"
// clean Imgui up and test
#include "..\..\imgui\imgui.h"
#include "..\..\imgui\imconfig.h"
#include "..\..\imgui\imgui_impl_win32.h"
#include "..\..\imgui\imgui_impl_dx12.h"

namespace Haku
{
void UILayer::OnAttach()
{
	ImGui::CreateContext();
	// check what this does
	auto io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	auto app		= Application::Get();
	auto hwnd		= app->GetWindow()->GetHandle();
	auto device		= app->GetRenderer()->GetDevice();
	auto descriptor = app->GetRenderer()->GetDesciptor();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		descriptor,
		descriptor->GetCPUDescriptorHandleForHeapStart(),
		descriptor->GetGPUDescriptorHandleForHeapStart());
}
void UILayer::Render()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	bool tr = true;
	ImGui::ShowDemoWindow(&tr);
	ImGui::Render();
}
void UILayer::OnDetach()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
} // namespace Haku
