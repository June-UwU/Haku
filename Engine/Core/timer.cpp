#include "timer.hpp"
#include "platform\platform.hpp"

HAPI void clock_start(struct timer& ptr)
{
	ptr.start_time	= platform_time();
	ptr.elapsed	= 0.0f;
}


HAPI void clock_update(struct timer& ptr)
{
	ptr.elapsed	+= platform_time(); 
}


HAPI void clock_stop(struct timer& ptr)
{
	ptr.elapsed	+= platform_time();
}

