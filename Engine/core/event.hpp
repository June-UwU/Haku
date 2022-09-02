#pragma once

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
	HK_KEY_UP,
	HK_KEY_DOWN,
	HK_MOUSE_BUTTON,
	HK_MOUSE_MOVE,
	HK_MOUSE_WHEEL,
	HK_SIZE,
	HK_MINIMIZE,

	MAX_EVENT_CODES // event code used to count the maximum number of event code
}event_code;

constexpr const char* HK_EVENT_STRING[MAX_EVENT_CODES]
{ 
	"exit event", 
	"key up event",
	"key down event",
	"mouse button event", 
	"mouse move",
	"mouse wheel",
	"size event"
	"minimize event"
};

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

// breif	peek the queue to find the next event type
HAPI event_code event_peek(void);

// @breif	context packing and unpacking functions that are used for the event systems

// @breif	packs two 16 bit values to generate a 32 bit value
// @param   : high 16 bits
// @param   : low 16 bits
// @return  : packed 32 bit value
constexpr u32 PACK_16BIT_VALS(u16 hiword, u16 loword)
{
	return ((hiword << 16) | loword);
}

// @breif	packs two 32 bit values to generate a 64 bit value
// @param   : high 32 bits
// @param   : low 32 bits
// @return  : packed 64 bit value
constexpr u64 PACK_32BIT_VALS(i32 hiword, i32 loword)
{
	u64 ret_val = ((hiword << 31) | loword);
	return ret_val;
}

// @breif	packs four 16 bit values to generate a 64 bit value
// @param   : high 16 bits
// @param   : mid high 16 bits
// @param   : mid low 16 bits
// @param	: low 16 bits
// @return  : packed 64 bit value
constexpr u64 PACK_64BIT_VALS(u16 hibyte, u16 mid_hi_byte, u16 mid_lo_byte, u16 lobyte)
{
	u32 loword = PACK_16BIT_VALS(mid_lo_byte, lobyte);
	u32 hiword = PACK_16BIT_VALS(hibyte, mid_hi_byte);
	u64 ret_val = PACK_32BIT_VALS(hiword, loword);
	return ret_val;
}

// @breif	returns the high word of the packed value
// @param   : packed 32 bit value
// @return  : high word of packed value
constexpr u16 HI_WORD(u32 pack_val)
{
	return (u16)(pack_val >> 16);
}

// @breif	returns the low word of the packed value
// @param   : packed 32 bit value
// @return  : low word of packed value
constexpr u16 LO_WORD(u32 pack_val)
{
	return (u16)pack_val;
}

// @breif	returns the high double word of the packed value
// @param   : packed 64 bit value
// @return  : high double word of packed value
constexpr u32 HI_DWORD(u64 pack_val)
{
	return (u32)(pack_val >> 32);
}

// @breif	returns the high double word of the packed value
// @param   : packed 64 bit value
// @return  : low double word of packed value
constexpr u32 LO_DWORD(u64 pack_val)
{
	return (u32)pack_val;
}
