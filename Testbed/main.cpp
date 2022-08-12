#include <Core/logger.hpp>



int main(void)
{
	HLEMER("Emergency %d",1);
	HLCRIT("Critical %d",2);
	HLERRO("Error %d",3);
	HLWARN("Warning %d",4);
	HLINFO("Information %d",5);

	return 0;
}
