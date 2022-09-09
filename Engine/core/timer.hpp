#pragma once

#include "defines.hpp"

// the timer deals with float in seconds
typedef struct timer
{
	f64 start_time; // start timer
	f64 elapsed;	// elapsed timer
} timer;

// @breif	routine to initialize the timer
// @param	: reference to timer
HAPI void clock_start(struct timer& ref);

// @breif	routine to update the timer
// @param	: reference to timer
HAPI void clock_update(struct timer& ref);

// @breif	routine to stop the timer
// @param	: reference to timer
HAPI void clock_stop(struct timer& ref);
