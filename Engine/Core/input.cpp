#include "input.hpp"

i8 input_system_initialize(void);

void input_system_shutdown(void);

void input_update(f64 delta_time);

HAPI i8 input_is_key_down(keys key);

HAPI i8 input_is_key_up(keys key);

HAPI i8 input_was_key_down(keys key);

HAPI i8 input_was_key_up(keys key);

void input_process_key(keys key, i8 pressed);

HAPI i8 input_is_button_down(buttons button);

HAPI i8 input_is_button_up(buttons button);

HAPI i8 input_was_button_down(buttons button);

HAPI i8 input_was_button_up(buttons button);

HAPI void input_get_mouse_position(i32* x, i32* y);

HAPI void input_get_previous_mouse_position(i32* x, i32* y);

void input_process_button(buttons button, i8 pressed);

void input_process_mouse_move(i16 x, i16 y);

