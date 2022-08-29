#pragma once

#include "defines.hpp"
#include <D3d12.h>
#include <dxgi1_6.h>


#define FRIENDLY_NAME(ID3D12Obj,name) ID3D12Obj->SetName(name)

constexpr u64 frame_count = 3u;


// different queue types
typedef enum
{
	HK_COMMAND_RENDER = 0x0,// render queue
	HK_COMMAND_COMPUTE,// compute queue
	HK_COMMAND_COPY,// copy queue

	HK_COMMAND_MAX

}queue_type;


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
	ID3D12Device* 		logical_device;			// logical device does all the gpu functions
	IDXGIFactory6*		factory;			// factory that creates the sub function
	IDXGIAdapter1* 		physical_device;		// the current physical device that is in use
	directx_swapchain   swapchain;
	directx_queue       queue;
}directx_context;
