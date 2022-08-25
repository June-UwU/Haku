#pragma once
// types that are used by renderer


#include "defines.hpp"

typedef enum
{
	HK_DIRECTX_12,
	HK_VULKAN,


	HK_BACKEND_MAX
}backends;

constexpr const char* BACKEND_STRING[HK_BACKEND_MAX]
{
	"DirectX 12",
	"Vulkan"
};

// @breif 	renderer backend abstract struct
typedef struct renderer_backend
{
	// @breif	backend type enum
	backends 	api_type;

	// @breif 	function pointer to initialize respective backends
	// @param 	: pointer to the backend struct that is filled out for further calls
	// @return	: H_OK on sucess
	i8 (*initialize)(renderer_backend* backend_ptr);

	// @breif	function pointer to the respective backend shutdown routine
	// @param 	: pointer to backend struct that is used for clean up
	void (*shutdown)(renderer_backend* backend_ptr);

	// @breif	function pointer to function that is initiating a frame
	// @param	: pointer to the backend
	// @param	: time elapsed
	// @return	: H_OK on sucess (this is allowed to fail and can reject to initiate a frame
	i8 (*begin_frame)(renderer_backend* backend_ptr,f64 delta_time);

	// @breif	function pointer to function that cleans up the function
	// @param	: pointer to the backend
	// @param	: delta time taken to complete the frame
	i8 (*end_frame)(renderer_backend* backend_ptr,f64 delta_time);

}renderer_backend;


// @breif 	a packet of information that is used to render each frame
typedef struct render_packet
{
	f64	delta_time;		// timing variable
}render_packet;
