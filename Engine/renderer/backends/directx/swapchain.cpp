#include "swapchain.hpp"
#include "core\logger.hpp"
#include "platform\platform.hpp"

//  TODO : refactor swapchain

typedef enum
{
	swapchain_query_fail,
	rtv_heap_creation_fail,
	association_fail,
	swapchain_fail,
}swapchain_fails;

i8 swapchain_fail_handler(directx_swapchain* swapchain, swapchain_fails fail_code,u64 context);

i8 swapchain_initialize(directx_context* context)
{

	i8 ret_code = H_OK;
	HRESULT  api_ret_code = S_OK;
	// initialization of nessary data
	// getting win32 platform specfic data
	void* handle;
	platform_data_for_render_api(&handle);

	p_prop plat_prop;
	get_platform_properties(&plat_prop);


	directx_queue* queue		= &context->queue;
	directx_swapchain* swapchain 	= &context->swapchain;
	swapchain->current_back_buffer_index = 0;
	// max buffer size for back buffer
	swapchain->max_in_filght_frames = frame_count - 1;


	DXGI_SWAP_CHAIN_DESC sdesc{};
	// swapa chain buffer desc
	sdesc.BufferDesc.Width 		= plat_prop.width;
	sdesc.BufferDesc.Height 	= plat_prop.height;
	sdesc.BufferCount = frame_count;
	sdesc.BufferDesc.RefreshRate.Numerator 		= 0u;
	sdesc.BufferDesc.RefreshRate.Denominator 	= 0u;
	sdesc.BufferDesc.Format 			= DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.BufferDesc.ScanlineOrdering 		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sdesc.BufferDesc.Scaling 			= DXGI_MODE_SCALING_UNSPECIFIED;

	// multi sampling conf , for now MULTI SAMPLING IS OFF
	sdesc.SampleDesc.Count 				= 1;
	sdesc.SampleDesc.Quality 			= 0;

	// buffer usage
	sdesc.BufferUsage 				= DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// swap effect
	sdesc.SwapEffect 				= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sdesc.Windowed 					= TRUE;
	sdesc.OutputWindow 				= *(HWND*)handle;

	IDXGISwapChain* tswapchain;
	api_ret_code = context->factory->CreateSwapChain(queue->directx_queue[HK_COMMAND_RENDER], &sdesc, &tswapchain);

	if (S_OK != api_ret_code)
	{
		HLEMER("swapchain creation failure");
		ret_code = swapchain_fail_handler(swapchain, swapchain_fail,0);
		return ret_code;
	}
	HLINFO("swap chain initailized");
	
	api_ret_code = tswapchain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swapchain->swapchain);
	if (S_OK != api_ret_code)
	{
		HLEMER("swapchain query failure");
		ret_code = swapchain_fail_handler(swapchain, swapchain_query_fail, 0);
		return ret_code;
	}
	HLINFO("swap chain queried");


	api_ret_code = context->factory->MakeWindowAssociation(*(HWND*)handle, DXGI_MWA_NO_WINDOW_CHANGES);
	if (S_OK != api_ret_code)
	{
		HLEMER("window assciation failure");
		ret_code = swapchain_fail_handler(swapchain, association_fail,0);
		return ret_code;
	}
	HLINFO("default behavior set");

	//  Rendertarget view heap properties
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frame_count;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


	api_ret_code = context->logical_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&swapchain->rtv_heap));
	if (S_OK != api_ret_code)
	{
		HLEMER("window assciation failure");
		ret_code = swapchain_fail_handler(swapchain, rtv_heap_creation_fail,0);
		return ret_code; 
	}

	swapchain->heap_increment = context->logical_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	// Create a RTV for each frame.
	for (u64 n = 0; n < frame_count; n++)
	{
		api_ret_code = swapchain->swapchain->GetBuffer(n, IID_PPV_ARGS(&swapchain->frame_resources[n]));
		if (S_OK != api_ret_code)
		{
			ret_code = swapchain_fail_handler(swapchain, rtv_heap_creation_fail, n + 1);
			return ret_code;
		}
		context->logical_device->CreateRenderTargetView(swapchain->frame_resources[n], nullptr, rtvHandle);
		rtvHandle.ptr += swapchain->heap_increment;
	}
	swapchain->current_back_buffer_index = swapchain->swapchain->GetCurrentBackBufferIndex();

	tswapchain->Release();

	return ret_code;

}

void swapchain_shutdown(directx_swapchain* swapchain)
{
	HLINFO("Releasing swap chain");
	swapchain->swapchain->Release();
	swapchain->rtv_heap->Release();
	for (int i = 0; i < frame_count; i++)
	{
		swapchain->frame_resources[i]->Release();
	}
}

i8 set_render_target(directx_swapchain* swapchain,directx_commandlist* commandlist)
{
	i8 ret_code = H_OK;

	swapchain->current_back_buffer_index = swapchain->swapchain->GetCurrentBackBufferIndex();

	ID3D12Resource* resource = swapchain->frame_resources[swapchain->current_back_buffer_index];

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandlist->commandlist->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	rtv_handle.ptr += (swapchain->current_back_buffer_index * swapchain->heap_increment);

	return ret_code;
}

i8 set_present_target(directx_swapchain* swapchain, directx_commandlist* commandlist)
{
	i8 ret_code = H_OK;

	ID3D12Resource* resource = swapchain->frame_resources[swapchain->current_back_buffer_index];

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	commandlist->commandlist->ResourceBarrier(1, &barrier);

	return ret_code;
}

void clear_back_buffer(directx_commandlist* commandlist,directx_swapchain* swapchain,f32 r, f32 g, f32 b, f32 a)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle =  swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	rtv_handle.ptr  +=  (swapchain->current_back_buffer_index * swapchain->heap_increment);

	f32 RGBA[4] = { r,g,b,a };

	commandlist->commandlist->ClearRenderTargetView(rtv_handle, RGBA, 0, nullptr);
}

i8 present_frame(directx_swapchain* swapchain)
{
	HRESULT api_ret_code = swapchain->swapchain->Present(1, 0);
	if (S_OK != api_ret_code)
	{
		HLEMER("failed to present swapchain");
		return H_FAIL;
	}
	return H_OK;
}



i8 swapchain_fail_handler(directx_swapchain* swapchain, swapchain_fails fail_code, u64 context)
{
	switch (fail_code)
	{
	case rtv_heap_creation_fail:
		for (int i = 0; i < context; i++)
		{
			swapchain->frame_resources[i]->Release();
		}
	case association_fail:
	swapchain_query_fail:
		swapchain->swapchain->Release();
	case swapchain_fail:
		break;
	}
	return H_FAIL;
}
