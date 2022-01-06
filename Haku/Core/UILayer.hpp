#pragma once
#include "../macros.hpp"
#include "../hakupch.hpp"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx12.h"
#include "../../imgui/imgui_impl_win32.h"

class Event;
namespace Haku
{
namespace UI
{
HAKU_API void  Begin() noexcept;
HAKU_API void  Render() noexcept;
HAKU_API void  InitUI() noexcept;
HAKU_API void  CleanUp() noexcept;
HAKU_API void  EndFrame() noexcept;
class HAKU_API DemoWindow
{
public:
	void Render();

private:
};
class HAKU_API Menubar
{
public:
	void Render() noexcept;

private:
	void File() noexcept;
};
class HAKU_API LeftMenu
{
public:
	void Render() noexcept;

private:
	int	  i0{};
	float col[3]{};
	float RotateX{};
	float RotateY{};
	float RotateZ{};
	bool  open = true;
};
} // namespace UI

} // namespace Haku
