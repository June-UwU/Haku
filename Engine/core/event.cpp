#include "event.hpp"
#include "logger.hpp"
#include "generics\darray.hpp"
#include "generics\c_queue.hpp"


// internal representation for event data 
typedef struct event_packet
{
	event_code	code;				// event code
	void*		sender;				// sender ptr
	int 		context;			// context data
}event_packet;

typedef struct event_entry
{
	void*  		listener;			// pointer to listener
	H_call_back 	callback;			// callback function pointer
}event_entry;

typedef struct event_table
{
	event_entry* 	table[H_ECODE_RANGE];		// table entry
}event_table;

constexpr u64		eq_capacity	= 256u;
static c_queue*		ep_queue 	= nullptr;
static event_table 	e_table;


i8 event_initialize(void)
{
	ep_queue	= create_c_queue(eq_capacity,sizeof(event_packet),true);	// this create a purging queue , attention must be given so as to not miss messages
	for(u64 i = 0; i < H_ECODE_RANGE; i++)
	{
		e_table.table[i]	= nullptr;
	}
	
	return H_OK;
}

i8 service_event(void)
{
	i8 ret_code = H_OK;
	while (0 != size(ep_queue))
	{
		event_packet* packet	= (event_packet*)dequeue(ep_queue);
		event_entry* event_vec  = e_table.table[packet->code];

		if (nullptr == event_vec)
		{
			HLWARN("No event vector code : %d", packet->code);
			return H_ERR;
		}

		for (u64 i = 0; i < size(event_vec); i++)
		{
			ret_code = event_vec[i].callback(packet->sender, packet->context);
		}
	}
	return ret_code;
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
	destroy_c_queue(ep_queue);
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
	i8 ret_code 	= H_OK;

	event_packet pack{};
	pack.code	= code;
	pack.sender	= sender;
	pack.context	= context;

	ret_code	= enqueue(ep_queue,&pack);

	return ret_code;
}
