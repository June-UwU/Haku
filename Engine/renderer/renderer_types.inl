/*****************************************************************//**
 * \file   renderer_types.inl
 * \brief  types and general defines and includes used by the renderer front-end
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/** enums that points the backend used */
typedef enum backends
{
	/** direct x 12 backend is used */
	HK_DIRECTX_12, 
	/** vulkan backend is used */
	HK_VULKAN,     

	/** maximum number of backends used */
	HK_BACKEND_MAX
}backends;

/** compile time string translation look up table for backends */
constexpr const char* BACKEND_STRING[HK_BACKEND_MAX]
{
	/** for HK_DIRECTX_12 */
	"DirectX 12",
	/** for HK_VULKAN */
	"Vulkan"
};

/** renderer backend representations structure */
typedef struct renderer_backend
{
	/** current frame count */
	u64         frame_count;
	/** backend type enum */
	backends 	api_type;

	/** function pointer to be implemented for each backend type for initializations */
	i8 (*initialize)(renderer_backend* backend_ptr);

	/** function pointer to be implemented for each backend type for shutdown */
	void (*shutdown)(renderer_backend* backend_ptr);

	/** function pointer to function that is initiating a frame */
	i8 (*begin_frame)(renderer_backend* backend_ptr,f64 delta_time);

	/** function pointer to function that cleans up the function */
	i8 (*end_frame)(renderer_backend* backend_ptr,f64 delta_time);

	/** function to handle resize events in the renderer_backend  */
	i8(*resize)(renderer_backend* backend_ptr, u16 height, u16 width);
}renderer_backend;

/** a frame's entire data needed for rendering */
typedef struct render_packet
{
	/** timing variable */
	f64	delta_time;		
}render_packet;
