#include "defines.hpp"
#include <cstdint>

// Haku Event Subsystem
// Event Context data : the context data is based on the ioctl type of parameter for linux, the context data may be casted on to a pointer or used as itself and is a 64 bit signed int

// @brief	event service call back routine 
// @param	sender, can be NULL but ideally be a pointer to whatever fired the event
// @param	context , linux ioctl type command that may or may not be casted to approiate pointer based on the use case
typedef i8 (*H_call_back)(void* sender, i64 context);

constexpr u64 H_ECODE_RANGE	= 	UINT_LEAST16_MAX;  // number of event codes 	

typedef enum
{
	HK_EXIT 	= 0x0,

	MAX_EVENT_CODES // event code used to count the maximum number of event code
}event_code;

// @brief	event subsystem initialization routine
// @returns 	: H_OK on sucess
i8 event_initialize(void);

// @brief	event service buffer
// @return 	: return H_OK once the event is handled
i8 service_event(void);

// @breif	event subsystem shutdown routine
void event_shutdown(void);

// @breif	event register code with function
// @param	: event code
// @param	: listener , pointer to listener , can be NULL
// @param	: callback function
// @return	: return H_OK on sucess , else H_FAIL
HAPI i8 event_register(event_code code,void* listener,H_call_back callback);

// @breif	event remove call back from call back
// @param	: event code
// @param	: pointer to listener
// @param	: callback function
// @return 	: returns H_OK if handled properly , else H_FAIL 
// @note	both the listener and callback will be used for finding the registered event
HAPI i8 event_unregister(event_code code,void* listener,H_call_back callback);

// @brief	event fire to be processed
// @param	: event code
// @param	: pointer to sender, call be NULL
// @param	: context to sent to the callback function
// @return 	: H_OK on sucess, else H_FAIL
HAPI i8 event_fire(event_code code, void* sender,i64 context);
