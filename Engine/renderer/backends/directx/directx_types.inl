/*****************************************************************//**
 * \file   directx_types.INL
 * \brief  directx implementations typedefs and other important macros
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

// TODO :ve the string lib 
#include "defines.hpp"
#include <DirectXMath.h>
#include <cwchar>
#include <D3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "generics/queue.hpp"

/** macro to set friendly names to directx 12 objects */
#define FRIENDLY_NAME(ID3D12Obj,name) ID3D12Obj->SetName(name)

/** frame count for the directx backend */
constexpr u64 frame_count = 3u;
/** command allcator size for the command allocator pool */
constexpr u64 command_allocator_size = 4u;

constexpr const D3D12_HEAP_PROPERTIES HEAP_PROPS[D3D12_HEAP_TYPE_CUSTOM]
{
	{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	},
	{
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	},
	{
		// READ BACK
    },  
    {
		// COSTOM HEAP
    }
};

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


typedef struct descriptor_block
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_base;

	D3D12_GPU_DESCRIPTOR_HANDLE gpu_base;

	u64 size;

}descriptor_block;

typedef struct directx_descriptor_heap
{
	queue_t block_list;

	ID3D12DescriptorHeap* heap;

	D3D12_CPU_DESCRIPTOR_HANDLE cpu_base;

	D3D12_GPU_DESCRIPTOR_HANDLE gpu_base;

	u64 heap_inc;

	u64 capacity;

	u64 largest_block;

	bool is_shader_visible;
}directx_descriptor_heap;

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


/** directx command allocators */
typedef struct directx_cc
{
	/** this val is the completed value when the allocator is off flight */
	u64 fence_val;

	/** current allocator state */
	command_buffer_state state;

	/** directx  12 specfic command allocator object */
	ID3D12CommandAllocator* allocator;

	/** assioated command list */
	ID3D12GraphicsCommandList* commandlist;
}directx_cc;


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
	ID3D12CommandQueue* directx_queue;
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
	/** direct queue object */
	directx_queue			queue;

	/** directx swapchain instance */
	directx_swapchain		swapchain;

	/** directx_device instance assioated with the context */
	directx_device          device;

	/** pointer that hold the current list working with*/
	directx_cc* curr_cc;
}directx_context;


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

/** haku's vertex types */
typedef struct hk_vertex
{
	/** variable that holds the point coordinates */
	DirectX::XMVECTOR point;

	/** variable that hold the color data */
	DirectX::XMVECTOR color;
}hk_vertex;

constexpr const u32 INPUT_ELEMENT_COUNT = 2u;

constexpr const D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC[2]
{
	{ "POSITION", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

constexpr const D3D12_INPUT_LAYOUT_DESC INPUT_LAYOUT_DESC
{
	INPUT_ELEMENT_DESC,
	INPUT_ELEMENT_COUNT
};
