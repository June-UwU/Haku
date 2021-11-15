#include <stdint.h>
class Application;

namespace Haku
{
class Window
{
	friend class Application;

public:
	Window(uint32_t height, uint32_t width, bool maximize, const char* Windowname);
	virtual void run() = 0;

private:
	Application*  Owner = nullptr;
	uint32_t height;
	uint32_t width;
	bool		  maximize;
};
} // namespace Haku
