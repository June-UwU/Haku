#pragma once

#include "Events.hpp"
#include "../hakupch.hpp"

class Application;

namespace Haku
{
class Window
{
	friend class Application;

protected:
	using EventRoutine = std::function<void(const Event& e)>;
	EventRoutine Routine;
	Window(uint32_t height, uint32_t width, bool maximize, const char* Windowname);

public:
	virtual void run() = 0;
	uint32_t	 GetWidth() { return width; }
	uint32_t	 GetHeight() { return height; }
	bool		 GetMinimize() { return minimize; }
	bool		 GetMaximize() { return maximize; }
	void		 SetWidth(const uint32_t value) { width = value; }
	void		 SetHeight(const uint32_t value) { height = value; }
	void		 SetMinimize(const bool value) { minimize = value; }
	void		 SetMaximize(const bool value) { maximize = value; }
	virtual void SetEventRoutine(const EventRoutine& func) = 0;

private:
	uint32_t width;
	uint32_t height;
	bool	 maximize = false;
	bool	 minimize = false;
};
} // namespace Haku
