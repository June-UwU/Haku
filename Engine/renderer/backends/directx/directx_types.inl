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
	/** @brief The command buffer is ready to begin. */
	COMMAND_BUFFER_STATE_READY,
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
	// the command list is ready to be reset with an allocator 
	COMMANDLIST_READY,
	
	//the command list  currently recording
	COMMANDLIST_RECORDING,

	// the command list is currently ended its recording
	COMMANDLIST_RECORDING_ENDED,

	// the command list is submitted for execution , it needs to resetted to be ready again
	COMMANDLIST_STALE
}commandlist_state;

typedef struct directx_commandlist
{
	commandlist_state state;

	ID3D12GraphicsCommandList* commandlist;
}directx_command_list;

typedef struct directx_allocator
{
	u64 fence_value;

	command_buffer_state state;

	ID3D12CommandAllocator* allocator;
}directx_allocator;

typedef struct directx_queue
{
	ID3D12CommandQueue* directx_queue[HK_COMMAND_MAX];
}directx_queue;

typedef struct directx_swapchain
{
	u64 heap_increment;

	u64 max_in_filght_frames;

	u64 current_back_buffer_index;
	
	IDXGISwapChain* swapchain;	// directx swap chain
	
	ID3D12Resource* frame_resources[frame_count];

	ID3D12DescriptorHeap* rtv_heap;

}directx_swapchain;

// directx renderer backend has releated information for a context
typedef struct directx_context
{
	ID3D12Device* 			logical_device;			// logical device does all the gpu functions
	IDXGIFactory6*			factory;			// factory that creates the sub function
	IDXGIAdapter1* 			physical_device;		// the current physical device that is in use
	directx_swapchain		swapchain;
	directx_queue			queue;
	directx_commandlist     commandlist[HK_COMMAND_MAX];
}directx_context;
