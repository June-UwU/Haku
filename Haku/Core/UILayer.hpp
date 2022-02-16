#pragma once
#include "../macros.hpp"
#include "../hakupch.hpp"
#include "imgui.h"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"

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
	void		  Render() noexcept;
	static float& GetFOV() noexcept;
	static float& GetFarZ() noexcept;
	static float& GetNearZ() noexcept;
	static float* GetRotateData() noexcept;
	static float& GetAspectRatio() noexcept;
	static void	  SetFOV(float val) noexcept;
	static float* GetTranslateData() noexcept;
	static void	  SetFarZ(float val) noexcept;
	static void	  SetNearZ(float val) noexcept;
	static void	  SetAspectRatio(float val) noexcept;

private:
	static float FOV;
	static float FarZ;
	static float NearZ;
	bool		 open = true;
	static float Aspect_Ratio;
	static float RotateXYZ[3];
	static float TranslationData[3];
};
} // namespace UI

} // namespace Haku
