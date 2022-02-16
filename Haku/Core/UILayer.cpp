#include "UILayer.hpp"
#include "Application.hpp"
namespace Haku
{
namespace UI
{
float LeftMenu::FOV;
float LeftMenu::FarZ;
float LeftMenu::NearZ;
float LeftMenu::Aspect_Ratio;
float LeftMenu::RotateXYZ[3];
float LeftMenu::TranslationData[3];

void InitUI() noexcept
{
	ImGui::CreateContext();
	auto* app			 = Haku::Application::Get();
	auto* hwnd			 = app->GetWindow()->GetHandle();
	auto* device		 = app->GetRenderer()->GetDevice();
	auto* descriptorheap = app->GetRenderer()->GetSRVDesciptor();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		descriptorheap,
		descriptorheap->GetCPUDescriptorHandleForHeapStart(),
		descriptorheap->GetGPUDescriptorHandleForHeapStart());
	ImGui::StyleColorsDark();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
void CleanUp() noexcept
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
HAKU_API void EndFrame() noexcept
{
	ImGui::EndFrame();
}
HAKU_API void Begin() noexcept
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
}
HAKU_API void Render() noexcept
{
	ImGui::Render();
}
void DemoWindow::Render()
{
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
}
void Menubar::File() noexcept
{
	ImGui::MenuItem("(demo menu)", NULL, false, false);
	if (ImGui::MenuItem("New"))
	{
	}
	if (ImGui::MenuItem("Open", "Ctrl+O"))
	{
	}
	if (ImGui::BeginMenu("Open Recent"))
	{
		if (ImGui::BeginMenu("More.."))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save", "Ctrl+S"))
	{
	}
	if (ImGui::MenuItem("Save As.."))
	{
	}

	ImGui::Separator();
	if (ImGui::BeginMenu("Options"))
	{
		static bool enabled = true;
		ImGui::MenuItem("Enabled", "", &enabled);
		ImGui::BeginChild("child", ImVec2(0, 60), true);
		for (int i = 0; i < 10; i++)
			ImGui::Text("Scrolling Text %d", i);
		ImGui::EndChild();
		static float f = 0.5f;
		static int	 n = 0;
		ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
		ImGui::InputFloat("Input", &f, 0.1f);
		ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Colors"))
	{
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
			ImVec2		p	 = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			ImGui::MenuItem(name);
		}
		ImGui::EndMenu();
	}
}
void Menubar::Render() noexcept
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			File();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z"))
			{
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
			{
			} // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X"))
			{
			}
			if (ImGui::MenuItem("Copy", "CTRL+C"))
			{
			}
			if (ImGui::MenuItem("Paste", "CTRL+V"))
			{
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
void LeftMenu::Render() noexcept
{
	if (ImGui::Begin("Parameters", &open, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar))
	{
		ImGui::InputFloat3("Rotation", RotateXYZ);
		ImGui::InputFloat3("Translation", TranslationData);
		ImGui::Text("Camera Parameters");
		ImGui::InputFloat("FOV", &FOV);
		ImGui::InputFloat("FarZ", &FarZ);
		ImGui::InputFloat("NearZ", &NearZ);
		ImGui::InputFloat("Aspect Ratio", &Aspect_Ratio);
		ImGui::End();
	}
}
float* LeftMenu::GetRotateData() noexcept
{
	return RotateXYZ;
}
float* LeftMenu::GetTranslateData() noexcept
{
	return TranslationData;
}
void LeftMenu::SetFOV(float val) noexcept
{
	FOV = val;
}
void LeftMenu::SetFarZ(float val) noexcept
{
	FarZ = val;
}
void LeftMenu::SetNearZ(float val) noexcept
{
	NearZ = val;
}
void LeftMenu::SetAspectRatio(float val) noexcept
{
	Aspect_Ratio = val;
}
float& LeftMenu::GetAspectRatio() noexcept
{
	return Aspect_Ratio;
}
float& LeftMenu::GetFOV() noexcept
{
	return FOV;
}
float& LeftMenu::GetFarZ() noexcept
{
	return FarZ;
}
float& LeftMenu::GetNearZ() noexcept
{
	return NearZ;
}
} // namespace UI
} // namespace Haku
