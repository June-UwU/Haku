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
	void		 SetWidth(uint32_t value) { width = value; }
	void		 SetHeight(uint32_t value) { height = value; }
	virtual void SetEventRoutine(const EventRoutine& func) = 0;

private:
	uint32_t width;
	uint32_t height;
	bool	 maximize;
};
} // namespace Haku
