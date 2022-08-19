#include "event.hpp"
#include "logger.hpp"
// TODO : make implementation



i8 event_initialize(void)
{
	HASSERT(false);
}

i8 service_event(void)
{
	HASSERT(false);
}

void event_shutdown(void)
{
	HASSERT(false);
}

i8 event_register(event_code code,void* listener,H_call_back callback)
{
	HASSERT(false);
}

i8 event_unregister(event_code code,void* listener,H_call_back callback)
{
	HASSERT(false);
}

i8 event_fire(event_code code, void* sender,i64 context)
{
	HASSERT(false);
}
