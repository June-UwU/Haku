/*****************************************************************//**
 * \file   directx_types.INL
 * \brief  directx implementations typedefs and other important macros
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

// TODO : remove the string lib 
#include <cwchar>

#include <hmath_types.hpp>
#include "defines.hpp"
#include <D3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "generics/darray.hpp"

/** macro to set friendly names to directx 12 objects */
#define FRIENDLY_NAME(ID3D12Obj,name) ID3D12Obj->SetName(name)

/** frame count for the directx backend */
constexpr u64 frame_count = 3u;
/** command allcator size for the command allocator pool */
constexpr u64 command_allocator_size = 4u;

// TODO : HAVE CPU ACCESSIBLE MODIFYING RESOURCES
/** resource type enumerations (THIS WILL PROLLY HAVE MODIFIABLE AND NON MODIFIABLE RESOURCES BASED ON THE TYPE USED) */
typedef enum resource_type
{
	/** defines a vertex resource */
	VERTEX_RESOURCE,

	/** defines a index resource */
	INDEX_RESOURCE,

	/** shader resource */
	SHADER_RESOURCE,

	/** constant resource */
	CONST_RESOURCE,

	/** unordered resource */
	UNORDERED_RESOURCE,

// TODO : CPU ACCESSIBLE RESOURCES GOES HERE
    /** this macro is used to temperorily make the comparison for CPU ACCESSIBLE RESOURCE  */
    CPU_ACCESS_STUB,

	/** flag to indicate that the resource is not yet ready for draw */
	RESOURCE_STALE_STUB,
    /** macro to make sure the max resource is taken into account */
	RESOURCE_TYPE_MAX
}resource_type;

/** a resource type to string array */
constexpr const char* RESOURCE_TYPE_STRING[RESOURCE_TYPE_MAX]
{
	"vertex resource",
	"index resource",
	"shader resource",
	"constant resource",
	"unordered resource"
};

/** struct to handle directx resources and their usage */
typedef struct directx_buffer
{
	/** resource_type enum */
	resource_type type;

	/** currently not used at the moment */
	D3D12_RESOURCE_STATES state;

	/** underlying resource */
	ID3D12Resource* resource;
}directx_buffer;


/** enumeration to signify the different programmable shader stage */
typedef enum shader_stages
{
	/** vertex shader stage */
	HK_VERTEX_SHADER,
	/** hull shader stage */
	HK_HULL_SHADER,
	/** domain shader stage */
	HK_DOMAIN_SHADER,
	/** geometry shader stage */
	HK_GEOMETRY_SHADER,
	/** pixel shader stage */
	HK_PIXEL_SHADER,
	/** compute shader stage */
	HK_COMPUTE_SHADER,

	/** max shader count */
	HK_SHADER_MAX
}shader_stages;

/** shader_stages enumration translation vector */
constexpr const char* SHADER_STAGE_NAME[HK_SHADER_MAX]
{
	"vertex shader",
	"hull shader",
	"domain shader",
	"geometry shader",
	"pixel shader",
	"compute shader"
};

/** enumeration to signify the different types of command list,queue and allocator types */
typedef enum  queue_type
{
	/** render queue */
	HK_COMMAND_RENDER = 0x0,
	/** compute queue */
	HK_COMMAND_COMPUTE,
	/** copy queue */
	HK_COMMAND_COPY,

	/** max queue type count */
	HK_COMMAND_MAX
}queue_type;

/** queue_type enumeration translation vector */
constexpr const wchar_t* COMMAND_NAME[HK_COMMAND_MAX]
{
	L"render",
	L"compute",
	L"copy"
};

/** command_buffer state signifying enumerations */
typedef enum command_buffer_state
{
	/** @brief The command buffer is currently being recorded to. */
	COMMAND_BUFFER_STATE_RECORDING,
	/** @brief The command buffer is has ended recording. */
	COMMAND_BUFFER_STATE_RECORDING_ENDED,
	/** @brief The command buffer has been submitted to the queue.can't be used unless its returned to the pool */
	COMMAND_BUFFER_STATE_SUBMITTED,
	/** @brief The command buffer is not allocated. */
	COMMAND_BUFFER_STATE_NOT_ALLOCATED
}command_buffer_state;


/** commandlist state signifying enumerations */
typedef enum commandlist_state
{
	/** the command list  currently recording */
	COMMANDLIST_RECORDING,

	/** the command list is currently ended its recording */
	COMMANDLIST_RECORDING_ENDED,

	/**  the command list is submitted for execution, it needs to resetted to be ready again */
	COMMANDLIST_STALE
}commandlist_state;

/** directx command allocators */
typedef struct directx_allocator
{
	/** allocator type */
	queue_type type;

	/** this val is the completed value when the allocator is off flight */
	u64 fence_val;

	/** current allocator state */
	command_buffer_state state;

	/** directx  12 specfic command allocator object */
	ID3D12CommandAllocator* allocator;
}directx_allocator;

/** directx commandlist , uses command allocators to record command list */
typedef struct directx_commandlist
{
	/** commandlist type */
	queue_type type;

	/**  command list current state */
	commandlist_state state;

	/** directx 12 command list object */
	ID3D12GraphicsCommandList* commandlist;

	/**  internal current allocator */
	directx_allocator* seeded_allocator;
}directx_command_list;

/** directx command queue used to execute command lists that are closed after recording */
typedef struct directx_queue
{
	/** current fence value */
	u64 fence_val;

	/** synchronizing objects */
	ID3D12Fence* fence;

	/**  Event to upgrade to for a gpu flush */
	HANDLE event;

	/** command queue list , equal to HK_COMMAND_MAX */
	ID3D12CommandQueue* directx_queue[HK_COMMAND_MAX];
}directx_queue;

/** directx swapchain object */
typedef struct directx_swapchain
{
	/** rtv heap increment size */
	u64 rtv_heap_increment;

	/** rtv heap increment size */
	u64 dsv_heap_increment;

	/** number of buffers that are possible off-flight */
	u64 max_in_filght_frames;

	/** current back buffer index in resource array */
	u64 current_back_buffer_index;
	
	/** directx swapchain object */
	IDXGISwapChain3* swapchain;	
	
	/** directx 12 depth stencil resource */
	ID3D12Resource* depth_stencil_resource;
	
	/** render target view resource */
	ID3D12Resource* frame_resources[frame_count];

	/** directx depth stencil optimized clear value */
	D3D12_CLEAR_VALUE depth_stencil_clear_value;

	/** directx scissor rect */
	D3D12_RECT scissor_rect;

	/** directx view port */
	D3D12_VIEWPORT viewport;

	/** directx descriptor heap for rendertarget views */
	ID3D12DescriptorHeap* rtv_heap;

	/** directx descriptor heap for depth stencil views */
	ID3D12DescriptorHeap* dsv_heap;
}directx_swapchain;

/** structure constructing the directx device  */
typedef struct directx_device
{
	/** factory that creates the sub function */
	IDXGIFactory6* factory;

	/** logical device does all the gpu functions */
	ID3D12Device* logical_device;

	/** the current physical device that is in use */
	IDXGIAdapter1* physical_device;

}directx_device;

/** directx renderer backend has releated information for a context */
typedef struct directx_context
{
	/** indicates whether the commandlist is ready for recording */
	bool is_ready[HK_COMMAND_MAX];

	/** direct queue object */
	directx_queue			queue;

	/** directx swapchain instance */
	directx_swapchain		swapchain;

	/** directx_device instance assioated with the context */
	directx_device          device;

	/** all directx commandlist types */
	directx_commandlist     commandlist[HK_COMMAND_MAX];
}directx_context;


/** rootsignature max capacity */
constexpr const u64 ROOT_PARAM_MAX = 64u;
/** root signature structure */
typedef struct directx_root_signature
{
	/** constant buffer view base register */
	u64 cbv_base;

	/** unordered access view base register */
	u64 uav_base;

	/** current shader resource view base */
	u64 srv_base;

	/** current sampler base register */
	u64 sampler_base;

	/** number of parameter in a root signature */
	u64 parameter_count;

	/** size of the parameter size allocated */
	u64 allocate_parameter;

	/** sampler array for carrying samplers */
	darray sampler_array;
	
	/** root parameter darray */
	darray parameter_array;
	
	/** root signature of directx api */
	ID3D12RootSignature* root_signature;

	/** default shader visibility used */
	D3D12_SHADER_VISIBILITY default_visibility;
}directx_root_signature;

/** directx resource types */
typedef enum descriptor_type
{
	/** constant buffer view */
	CBV,
	
	/** unordered buffer view */
	UAV,

	/** shader resource view */
	SRV,

	/** sampler */
	SAMPLER,

	/** maximum descriptor count */
	DESCRIPTOR_COUNT

}descriptor_type;


/** directx shader module  */
typedef struct directx_shader_module
{
	/** all directx shader for a module, nullptr if its absent */
	ID3DBlob* compiled_shaders[HK_SHADER_MAX];
}directx_shader_module;

/** directx pipeline state structure */
typedef struct directx_pipeline
{
	/** the pipeline state created */
	ID3D12PipelineState* state;

	/** the shader structure */
	directx_shader_module* shaders;

	/** the root signature assioated with the pipeline */
	directx_root_signature* signature;
}directx_pipeline;


/** haku's vertex types */
typedef struct hk_vertex
{
	/** variable that holds the point coordinates */
	vector3d point;

	/** variable that hold the color data */
	vector4d color;
}hk_vertex;
