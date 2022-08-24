#include "input.hpp"
#include "event.hpp"


// mouse scroll delta +ve means scroll up and -v means (MAY EVEN DO A CAP CUT OFF)
static i32 	mouse_delta;

// mouse prev data
static i32 	mouse_prev_x;			// prev x position
static i32	mouse_prev_y;			// prev y position
static bool	mouse_prev[BUTTON_MAX_BUTTONS]; // button array

// mouse current data
static i32	mouse_current_x;		// current x position
static i32	mouse_current_y;		// current y position
static bool 	mouse_current[BUTTON_MAX_BUTTONS]; //  current button state

// keyboard current and previous state
static bool keyboard_prev[KEYS_MAX_KEYS];
static bool keyboard_current[KEYS_MAX_KEYS];


i8 input_system_initialize(void)
{
	mouse_delta	= 0;
	mouse_prev_x	= 0;
	mouse_prev_y	= 0;
	mouse_current_x	= 0;
	mouse_current_y	= 0;

	for(u64 i = 0; i < BUTTON_MAX_BUTTONS; i++)
	{
		mouse_prev[i]	= false;
		mouse_current[i]= false;
	}

	for(u64 i = 0; i < KEYS_MAX_KEYS; i++)
	{
		keyboard_prev[i]= false;
		keyboard_current[i]= false;
	}
	return H_OK;
}

void input_system_shutdown(void)
{

}

void input_update(f64 delta_time)
{
	// TODO : make sure the delta reset make logical sense
	mouse_delta		= 0;

	mouse_prev_x		= mouse_current_x;
	mouse_prev_y		= mouse_current_y;

	for(u64 i = 0; i < BUTTON_MAX_BUTTONS; i++)
	{
		mouse_prev[i]	= mouse_current[i];
		mouse_current[i]= false;
	}

	for(u64 i = 0; i < KEYS_MAX_KEYS; i++)
	{
		keyboard_prev[i]= keyboard_current[i];
		keyboard_current[i]= false;
	}
}

i8 input_is_key_down(keys key)
{
	return keyboard_current[key];
}

i8 input_is_key_up(keys key)
{
	return !(keyboard_current[key]);
}

i8 input_was_key_down(keys key)
{
	return keyboard_prev[key];
}

i8 input_was_key_up(keys key)
{	
	return !(keyboard_prev[key]);
}

void input_process_key(keys key, bool pressed)
{
	keyboard_current[key] = pressed;
	if (true == pressed)
	{
		event_fire(HK_KEY_DOWN, nullptr, key); // the context data is the code and the function is expected to query from the input subsystem for further processing
	}
	else
	{
		event_fire(HK_KEY_UP, nullptr, key); 
	}
}

i8 input_is_button_down(buttons button)
{
	return mouse_current[button];
}

i8 input_is_button_up(buttons button)
{
	return  !(mouse_current[button]);
}

i8 input_was_button_down(buttons button)
{
	return mouse_prev[button];
}

i8 input_was_button_up(buttons button)
{
	return !(mouse_prev[button]);
}

void input_get_mouse_position(i32* x, i32* y)
{
	*x 	= mouse_current_x;
	*y	= mouse_current_y;
}

void input_get_previous_mouse_position(i32* x, i32* y)
{
	*x 	= mouse_prev_x;
	*y	= mouse_prev_y;
}

void input_process_button(buttons button, i8 pressed)
{
	mouse_current[button]	= pressed;
	event_fire(HK_MOUSE_BUTTON,nullptr,button);
}

void input_process_mouse_move(i16 x, i16 y)
{
	mouse_current_x	= x;
	mouse_current_y	= y;
	event_fire(HK_MOUSE_MOVE, nullptr, 0);
}

void input_process_mouse_wheel(i8 z_delta)
{
	mouse_delta	= z_delta;
	event_fire(HK_MOUSE_WHEEL, nullptr, 0);
}
