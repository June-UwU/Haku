#include "UILayer.hpp"
#include "Application.hpp"
namespace Haku
{
namespace UI
{
void InitUI() noexcept
{
	ImGui::CreateContext();
	auto* app		 = Haku::Application::Get();
	auto* hwnd		 = app->GetWindow()->GetHandle();
	auto* device	 = app->GetRenderer()->GetDevice();
	auto* descriptor = app->GetRenderer()->GetDesciptor();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		descriptor,
		descriptor->GetCPUDescriptorHandleForHeapStart(),
		descriptor->GetGPUDescriptorHandleForHeapStart());
	ImGui::StyleColorsDark();
}
void CleanUp() noexcept
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void DemoWindow::Render()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::Render();
}
} // namespace UI
} // namespace Haku
