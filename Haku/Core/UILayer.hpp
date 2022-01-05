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
HAKU_API void  InitUI() noexcept;
HAKU_API void  CleanUp() noexcept;
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
} // namespace UI

} // namespace Haku
