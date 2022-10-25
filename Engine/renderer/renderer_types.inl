/*****************************************************************//**
 * \file   renderer_types.inl
 * \brief  types and general defines and includes used by the renderer front-end
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include <DirectXMath.h>
 /*

per model  transformations (model space -> model space) -> 
world transformations (model space -> world space) ->
projection transformation (camera views) -> 
view port thing is done by dx12??

*/

/** this is a unique 64 bit value kept in track by the renderer backend and is used by the frontend */
typedef u64 resource_uid;

/** global transform to apply to all meshes */
typedef struct global_transforms
{
	/** projection matrix ()*/
	DirectX::XMMATRIX projection_matix;

	/** world transform of meshes */
	DirectX::XMMATRIX view_matrix;

	/** padding for 256bytes alignment that can be expanded upon */
	i8 reserved_data[128];
}global_transforms;


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
	i8 (*initialize)(renderer_backend* backend_ptr,void* data);

	/** function pointer to be implemented for each backend type for shutdown */
	void (*shutdown)(renderer_backend* backend_ptr);

	/** function pointer to function that is initiating a frame */
	i8 (*begin_frame)(renderer_backend* backend_ptr,f64 delta_time);

	/** function pointer to function that cleans up the function */
	i8 (*end_frame)(renderer_backend* backend_ptr,f64 delta_time);

	/** function to handle resize events in the renderer_backend  */
	i8(*resize)(renderer_backend* backend_ptr, u16 height, u16 width);

	/** function to update global transform */
	i8(*update_global_transforms)(renderer_backend* backend_ptr,void* transform);

	/** current fov */
	f32 fov;

	/** current height */
	i32 height;

	/** current width */
	i32 width;

	/** a global transform that is applied to every model */
	global_transforms transforms;
}renderer_backend;


/** a frame's entire data needed for rendering */
typedef struct render_packet
{
	/** current feild of view */
	f32 fov;

	/** current width */
	i32 width;

	/** current height */
	i32 height;

	/** timing variable */
	f64	delta_time;		
}render_packet;
