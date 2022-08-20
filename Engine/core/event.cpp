#include "event.hpp"
#include "logger.hpp"
#include "generics\darray.hpp"

typedef struct event_entry
{
	void*  		listener;			// pointer to listener
	H_call_back 	callback;			// callback function pointer
}event_entry;

typedef struct event_table
{
	event_entry* 	table[H_ECODE_RANGE];		// table entry
}event_table;

static event_table 	e_table;

// TODO : implement a buffered array of events maybe a circular array of events that eats other events if needed?
// TODO : implement insertat and removeat for darray

i8 event_initialize(void)
{

	for(u64 i = 0; i < H_ECODE_RANGE; i++)
	{
		e_table.table[i]	= nullptr;
	}
	
	return H_OK;
}

i8 service_event(void)
{
	//HASSERT(false);
	return H_OK;
}

void event_shutdown(void)
{
	for(u64 i = 0; i < H_ECODE_RANGE; i++)
	{
		if(nullptr != e_table.table[i])
		{
			destroy_darray(e_table.table[i]);
		}
	}
}

i8 event_register(event_code code,void* listener,H_call_back callback)
{
	event_entry entry{};
	entry.callback 			= callback;
	entry.listener			= listener;

	if(nullptr == e_table.table[code])
	{
		e_table.table[code] 	= reinterpret_cast<event_entry*>(create_darray(sizeof(struct  event_entry)));
		e_table.table[code]	= reinterpret_cast<event_entry*>(push_back(e_table.table[code],&entry));
	}
	else
	{
		e_table.table[code]	= reinterpret_cast<event_entry*>(push_back(e_table.table[code],&entry));
	}
	return H_OK;
}

i8 event_unregister(event_code code,void* listener,H_call_back callback)
{
	event_entry* event_vec		= e_table.table[code];

	if( nullptr == event_vec)
	{
		HLCRIT(" Calling unregister on a null code");
		return H_ERR;
	}
	
	for(u64 i = 0; i < size(event_vec); i++)
	{
		if(event_vec[i].listener == listener && event_vec[i].callback == callback)
		{
			event_vec 	= reinterpret_cast<event_entry*>( remove_at(event_vec,i));
			return H_OK;
		}
	}

	HLCRIT(" Event specified is not found , Check registations point..!");
	return H_ERR;
}

// TODO : make the sender and listener check for event dispatch
i8 event_fire(event_code code, void* sender,i64 context)
{
	event_entry* event_vec		= e_table.table[code];
	i8 ret_code 			= H_OK;


	if( nullptr == event_vec)
	{
		HLWARN("No event vector code : %d",code);
		return H_ERR;
	}


	for(u64 i = 0; i < size(event_vec); i++)
	{
		ret_code = event_vec[i].callback(sender,context);
	}
	return ret_code;
}
