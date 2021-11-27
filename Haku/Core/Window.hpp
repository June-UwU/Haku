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
	Window(uint32_t height, uint32_t width, bool maximize, const char* Windowname);
	EventRoutine Routine;

public:
	virtual void run()									   = 0;
	virtual void SetEventRoutine(const EventRoutine& func) = 0;
	uint32_t	 GetHeight() { return height; }
	uint32_t	 GetWidth() { return width; }

private:
	uint32_t height;
	uint32_t width;
	bool	 maximize;
};
} // namespace Haku
