#include <Core/logger.hpp>



int main(void)
{
	HLEMER("Emergency %d",1);
	HLCRIT("Critical %d",2);
	HLERRO("Error %d",3);
	HLWARN("Warning %d",4);
	HLINFO("Information %d",5);
	HLEMER("Emergency %d", 6);
	HLCRIT("Critical %d", 7);
	HLERRO("Error %d", 8);
	HLWARN("Warning %d", 9);
	HLINFO("Information %d", 10);
	HLWARN("This is my message ");
	HASSERT(false);
	return 0;
}
