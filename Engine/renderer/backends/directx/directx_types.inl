#pragma once

// TODO : remove the string lib 
#include <cwchar>

#include "defines.hpp"
#include <D3d12.h>
#include <dxgi1_6.h>


#define FRIENDLY_NAME(ID3D12Obj,name) ID3D12Obj->SetName(name)

constexpr u64 frame_count = 3u;
constexpr u64 command_allocator_size = 4u;
// different queue types
typedef enum
{
	HK_COMMAND_RENDER = 0x0,// render queue
	HK_COMMAND_COMPUTE,// compute queue
	HK_COMMAND_COPY,// copy queue

	HK_COMMAND_MAX

}queue_type;


constexpr const wchar_t* COMMAND_NAME[HK_COMMAND_MAX]
{
	L"render",
	L"compute",
	L"copy"
};

typedef enum
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

typedef enum
{
	//the command list  currently recording
	COMMANDLIST_RECORDING,

	// the command list is currently ended its recording
	COMMANDLIST_RECORDING_ENDED,

	// the command list is submitted for execution , it needs to resetted to be ready again
	COMMANDLIST_STALE
}commandlist_state;


typedef struct directx_allocator
{
	//allocator type
	queue_type type;

	// this val is the completed value when the allocator is off flight
	u64 fence_val;

	// allocator state
	command_buffer_state state;

	ID3D12CommandAllocator* allocator;
}directx_allocator;


typedef struct directx_commandlist
{
	queue_type type;

	// command list current state
	commandlist_state state;

	// command list object
	ID3D12GraphicsCommandList* commandlist;

	// current allocator
	directx_allocator* seeded_allocator;
}directx_command_list;

typedef struct directx_queue
{
	// current fence value
	u64 fence_val;

	// synchronizing objects
	ID3D12Fence* fence;

	// Event to upgrade to for a gpu flush
	HANDLE event;

	// command queue list 
	ID3D12CommandQueue* directx_queue[HK_COMMAND_MAX];
}directx_queue;

typedef struct directx_swapchain
{
	// rtv heap increment size
	u64 rtv_heap_increment;

	// rtv heap increment size
	u64 dsv_heap_increment;

	// number of write buffer
	u64 max_in_filght_frames;

	// current back buffer
	u64 current_back_buffer_index;
	
	IDXGISwapChain3* swapchain;	// directx swap chain
	
	ID3D12Resource* depth_stencil_resource;
	
	ID3D12Resource* frame_resources[frame_count];

	D3D12_CLEAR_VALUE depth_stencil_clear_value;

	D3D12_RECT scissor_rect;

	D3D12_VIEWPORT viewport;

	// rtv heap
	ID3D12DescriptorHeap* rtv_heap;

	// dsv heap
	ID3D12DescriptorHeap* dsv_heap;
}directx_swapchain;

// directx renderer backend has releated information for a context
typedef struct directx_context
{
	directx_queue			queue;
	IDXGIFactory6*			factory;			// factory that creates the sub function
	directx_swapchain		swapchain;
	ID3D12Device* 			logical_device;			// logical device does all the gpu functions
	IDXGIAdapter1* 			physical_device;		// the current physical device that is in use
	directx_commandlist     commandlist[HK_COMMAND_MAX];
}directx_context;
