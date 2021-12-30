#include "UIapi.hpp"
#include "../../vendor/imgui/backends/imgui_impl_dx12.h"
#include "../../vendor/imgui/backends/imgui_impl_win32.h"

namespace Haku
{
void UI::render()
{
	ImGui::Render();
}
void UI::DarkMode()
{
	ImGui::StyleColorsDark();
}
void UI::DX12_newframe()
{
	ImGui_ImplDX12_NewFrame();
}
void UI::CreateContext()
{
	ImGui::CreateContext();
}
void UI::DX12_shutdown()
{
	ImGui_ImplDX12_Shutdown();
}
void UI::win32_shutdown()
{
	ImGui_ImplWin32_Shutdown();
}
void UI::DestroyContext()
{
	ImGui::DestroyContext();
}
void UI::win32_newframe()
{
	ImGui_ImplWin32_NewFrame();
}
void UI::generic_new_frame()
{
	ImGui::NewFrame();
}
void UI::win32_Init(void* hwnd)
{
	ImGui_ImplWin32_Init(hwnd);
}
void UI::DX12_Init(
	ID3D12Device*				device,
	int							num_frames_in_flight,
	DXGI_FORMAT					rtv_format,
	ID3D12DescriptorHeap*		cbv_srv_heap,
	D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle,
	D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle)
{
	ImGui_ImplDX12_Init(
		device,
		num_frames_in_flight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		cbv_srv_heap,
		font_srv_cpu_desc_handle,
		font_srv_gpu_desc_handle);
}
void UI::DemoWindow(bool cond)
{
	ImGui::ShowDemoWindow(&cond);
}
}; // namespace Haku
