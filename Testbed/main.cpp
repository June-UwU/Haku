#include <Core/logger.hpp>
#include <Platform/platform.hpp>



int main(void)
{
	platform_initialize(nullptr, "Haku Test", 100, 100, 720, 1080);
	HLEMER("Emergency %d",1);
	HLCRIT("Critical %d",2);
	HLERRO("Error %d",3);
	HLWARN("Warning %d",4);
	HLINFO("Information %d",5);
	while(true)
	{
		platform_pump_messages();
	}
	platform_shutdown();
	return 0;
}
