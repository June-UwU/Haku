/*****************************************************************/ /**
																	 * \file   swapchain.cpp
																	 * \brief  directx swapchain and support system
																	 *implementations
																	 *
																	 * \author June
																	 * \date   September 2022
																	 *********************************************************************/
#include "misc.hpp"
#include "swapchain.hpp"
#include "core\logger.hpp"
#include "platform\platform.hpp"

//  TODO : refactor swapchain

/** swapchain creation failure handling codes */
typedef enum swapchain_fails
{
	/** depth stencil resource creation failure */
	dsv_resource_creation_fail,
	/** depth stencil creation failure */
	dsv_heap_creation_fail,
	/** swapchain query failure to IDXGISwapChain3 */
	swapchain_query_fail,
	/** render target view creation failure */
	rtv_heap_creation_fail,
	/** window assioation failure */
	association_fail,
	/** swapchain general failure */
	swapchain_fail,
} swapchain_fails;

/**
 * swapchain creation failure handling routine.
 *
 * \param swapchain[in] pointer to directx_swapchain
 * \param fail_code[in] failure code
 * \param context arbitary code that is give to routine this will be used based on the fail_code
 * \return H_FAIL
 */
i8 swapchain_fail_handler(directx_swapchain* swapchain, swapchain_fails fail_code, u64 context);

i8 swapchain_initialize(directx_context* context)
{
	i8		ret_code	 = H_OK;
	HRESULT api_ret_code = S_OK;
	// initialization of nessary data
	// getting win32 platform specfic data
	void* handle;
	platform_data_for_render_api(&handle);

	p_prop plat_prop;
	get_platform_properties(&plat_prop);

	directx_device*	   device			 = &context->device;
	directx_queue*	   queue			 = &context->queue;
	directx_swapchain* swapchain		 = &context->swapchain;
	swapchain->current_back_buffer_index = 0;
	// max buffer size for back buffer
	swapchain->max_in_filght_frames = frame_count - 1;

	swapchain->viewport.Height	 = plat_prop.height;
	swapchain->viewport.Width	 = plat_prop.width;
	swapchain->viewport.MinDepth = 0.0f;
	swapchain->viewport.MaxDepth = 1.0f;
	swapchain->viewport.TopLeftX = 0;
	swapchain->viewport.TopLeftY = 0;

	DXGI_SWAP_CHAIN_DESC sdesc{};
	// swapa chain buffer desc
	sdesc.BufferDesc.Width					 = plat_prop.width;
	sdesc.BufferDesc.Height					 = plat_prop.height;
	sdesc.BufferCount						 = frame_count;
	sdesc.BufferDesc.RefreshRate.Numerator	 = 0u;
	sdesc.BufferDesc.RefreshRate.Denominator = 0u;
	sdesc.BufferDesc.Format					 = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.BufferDesc.ScanlineOrdering		 = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sdesc.BufferDesc.Scaling				 = DXGI_MODE_SCALING_UNSPECIFIED;

	// multi sampling conf , for now MULTI SAMPLING IS OFF
	sdesc.SampleDesc.Count	 = 1;
	sdesc.SampleDesc.Quality = 0;

	// buffer usage
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// swap effect
	sdesc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sdesc.Windowed	   = TRUE;
	sdesc.OutputWindow = *(HWND*)handle;

	IDXGISwapChain* tswapchain;
	api_ret_code = device->factory->CreateSwapChain(queue->directx_queue[HK_COMMAND_RENDER], &sdesc, &tswapchain);

	if (S_OK != api_ret_code)
	{
		HLEMER("swapchain creation failure");
		ret_code = swapchain_fail_handler(swapchain, swapchain_fail, 0);
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

	api_ret_code = device->factory->MakeWindowAssociation(*(HWND*)handle, DXGI_MWA_NO_WINDOW_CHANGES);
	if (S_OK != api_ret_code)
	{
		HLEMER("window assciation failure");
		ret_code = swapchain_fail_handler(swapchain, association_fail, 0);
		return ret_code;
	}
	HLINFO("default behavior set");

	//  Rendertarget view heap properties
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors			   = frame_count;
	rtvHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	api_ret_code = device->logical_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&swapchain->rtv_heap));
	if (S_OK != api_ret_code)
	{
		HLEMER("window assciation failure");
		ret_code = swapchain_fail_handler(swapchain, rtv_heap_creation_fail, 0);
		return ret_code;
	}
	FRIENDLY_NAME(swapchain->rtv_heap, L"render target view heap");
	swapchain->rtv_heap_increment =
		device->logical_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	// Create a RTV for each frame.
	wchar_t rtv_res_name[256u];
	for (u64 n = 0; n < frame_count; n++)
	{
		api_ret_code = swapchain->swapchain->GetBuffer(n, IID_PPV_ARGS(&swapchain->frame_resources[n]));
		if (S_OK != api_ret_code)
		{
			ret_code = swapchain_fail_handler(swapchain, rtv_heap_creation_fail, n + 1);
			return ret_code;
		}
		swprintf(rtv_res_name, 256u, L"rtv resource % lld", n);
		device->logical_device->CreateRenderTargetView(swapchain->frame_resources[n], nullptr, rtvHandle);
		FRIENDLY_NAME(swapchain->frame_resources[n], rtv_res_name);
		rtvHandle.ptr += swapchain->rtv_heap_increment;
	}
	swapchain->current_back_buffer_index = swapchain->swapchain->GetCurrentBackBufferIndex();

	tswapchain->Release();

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors			   = 1;
	dsvHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	api_ret_code = device->logical_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&swapchain->dsv_heap));
	if (S_OK != api_ret_code)
	{
		HLEMER("failed to create dsv heap");
		ret_code = swapchain_fail_handler(swapchain, dsv_heap_creation_fail, frame_count);
		return ret_code;
	}
	FRIENDLY_NAME(swapchain->dsv_heap, L"depth stencil view heap");
	swapchain->dsv_heap_increment =
		device->logical_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
	depth_stencil_view_desc.Format						  = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_view_desc.ViewDimension				  = D3D12_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags						  = D3D12_DSV_FLAG_NONE;

	swapchain->depth_stencil_clear_value.Format				  = DXGI_FORMAT_D32_FLOAT;
	swapchain->depth_stencil_clear_value.DepthStencil.Depth	  = 1.0f;
	swapchain->depth_stencil_clear_value.DepthStencil.Stencil = 0;

	D3D12_RESOURCE_DESC dsv_desc{};
	dsv_desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsv_desc.Format				= DXGI_FORMAT_D32_FLOAT;
	dsv_desc.MipLevels			= 0;
	dsv_desc.Alignment			= 0;
	dsv_desc.Width				= plat_prop.width;
	dsv_desc.Height				= plat_prop.height;
	dsv_desc.DepthOrArraySize	= 1;
	dsv_desc.SampleDesc.Count	= 1;
	dsv_desc.SampleDesc.Quality = 0;
	dsv_desc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES dsv_heap_prop{};
	dsv_heap_prop.Type				   = D3D12_HEAP_TYPE_DEFAULT;
	dsv_heap_prop.CPUPageProperty	   = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	dsv_heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	dsv_heap_prop.CreationNodeMask	   = 0;
	dsv_heap_prop.VisibleNodeMask	   = 0;

	api_ret_code = device->logical_device->CreateCommittedResource(
		&dsv_heap_prop,
		D3D12_HEAP_FLAG_NONE,
		&dsv_desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&swapchain->depth_stencil_clear_value,
		IID_PPV_ARGS(&swapchain->depth_stencil_resource));

	device->logical_device->CreateDepthStencilView(
		swapchain->depth_stencil_resource,
		&depth_stencil_view_desc,
		swapchain->dsv_heap->GetCPUDescriptorHandleForHeapStart());

	if (S_OK != api_ret_code)
	{
		HLEMER("failed to create dsv resource");
		ret_code = swapchain_fail_handler(swapchain, dsv_resource_creation_fail, frame_count);
		return ret_code;
	}

	swapchain->viewport		= { 0.0f, 0.0f, static_cast<f32>(plat_prop.width), static_cast<f32>(plat_prop.height),
								0.0f, 1.0f };
	swapchain->scissor_rect = { 0, 0, (i32)plat_prop.width, (i32)plat_prop.height };

	return ret_code;
}

void swapchain_shutdown(directx_swapchain* swapchain)
{
	HLINFO("Releasing swap chain");
	swapchain->swapchain->Release();
	swapchain->rtv_heap->Release();
	swapchain->dsv_heap->Release();
	for (int i = 0; i < frame_count; i++)
	{
		swapchain->frame_resources[i]->Release();
	}
	swapchain->depth_stencil_resource->Release();
}

i8 set_render_target(directx_swapchain* swapchain, directx_commandlist* commandlist)
{
	i8 ret_code = H_OK;

	swapchain->current_back_buffer_index = swapchain->swapchain->GetCurrentBackBufferIndex();

	ID3D12Resource* resource = swapchain->frame_resources[swapchain->current_back_buffer_index];

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resource;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandlist->commandlist->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	rtv_handle.ptr += (swapchain->current_back_buffer_index * swapchain->rtv_heap_increment);

	return ret_code;
}

i8 set_present_target(directx_swapchain* swapchain, directx_commandlist* commandlist)
{
	i8 ret_code = H_OK;

	ID3D12Resource* resource = swapchain->frame_resources[swapchain->current_back_buffer_index];

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resource;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandlist->commandlist->ResourceBarrier(1, &barrier);

	return ret_code;
}

void clear_back_buffer(directx_commandlist* commandlist, directx_swapchain* swapchain, f32 r, f32 g, f32 b, f32 a)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	rtv_handle.ptr += (swapchain->current_back_buffer_index * swapchain->rtv_heap_increment);

	f32 RGBA[4] = { r, g, b, a };

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

i8 swapchain_resize(directx_context* context, directx_swapchain* swapchain, u16 width, u16 height)
{
	HRESULT api_ret_code = S_OK;

	directx_device* device = &context->device;

	u16 min_width  = 8u;
	u16 min_height = 8u;

	width  = hmax(width, min_width);
	height = hmax(min_height, height);

	for (size_t i = 0; i < frame_count; i++)
	{
		swapchain->frame_resources[i]->Release();
	}
	swapchain->depth_stencil_resource->Release();

	DXGI_SWAP_CHAIN_DESC desc{};
	api_ret_code = swapchain->swapchain->GetDesc(&desc);
	if (S_OK != api_ret_code)
	{
		HLEMER("failed to get the directx swapchain descriptions");
		return H_FAIL;
	}

	api_ret_code = swapchain->swapchain->ResizeBuffers(frame_count, width, height, desc.BufferDesc.Format, desc.Flags);
	if (S_OK != api_ret_code)
	{
		HLEMER("failed to get the directx swapchain descriptions");
		return H_FAIL;
	}
	swapchain->current_back_buffer_index = swapchain->swapchain->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	wchar_t rtv_res_name[256u];
	for (u64 n = 0; n < frame_count; n++)
	{
		api_ret_code = swapchain->swapchain->GetBuffer(n, IID_PPV_ARGS(&swapchain->frame_resources[n]));
		if (S_OK != api_ret_code)
		{
			HLEMER("swapchain buffer failed");
			return H_FAIL;
		}
		swprintf(rtv_res_name, 256u, L"rtv resource % lld", n);
		device->logical_device->CreateRenderTargetView(swapchain->frame_resources[n], nullptr, rtvHandle);
		FRIENDLY_NAME(swapchain->frame_resources[n], rtv_res_name);
		rtvHandle.ptr += swapchain->rtv_heap_increment;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
	depth_stencil_view_desc.Format						  = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_view_desc.ViewDimension				  = D3D12_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags						  = D3D12_DSV_FLAG_NONE;

	swapchain->depth_stencil_clear_value.Format				  = DXGI_FORMAT_D32_FLOAT;
	swapchain->depth_stencil_clear_value.DepthStencil.Depth	  = 1.0f;
	swapchain->depth_stencil_clear_value.DepthStencil.Stencil = 0;

	D3D12_RESOURCE_DESC dsv_desc{};
	dsv_desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsv_desc.Format				= DXGI_FORMAT_D32_FLOAT;
	dsv_desc.MipLevels			= 0;
	dsv_desc.Alignment			= 0;
	dsv_desc.Width				= width;
	dsv_desc.Height				= height;
	dsv_desc.DepthOrArraySize	= 1;
	dsv_desc.SampleDesc.Count	= 1;
	dsv_desc.SampleDesc.Quality = 0;
	dsv_desc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES dsv_heap_prop{};
	dsv_heap_prop.Type				   = D3D12_HEAP_TYPE_DEFAULT;
	dsv_heap_prop.CPUPageProperty	   = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	dsv_heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	dsv_heap_prop.CreationNodeMask	   = 0;
	dsv_heap_prop.VisibleNodeMask	   = 0;

	swapchain->depth_stencil_clear_value.DepthStencil.Depth	  = 1.0f;
	swapchain->depth_stencil_clear_value.DepthStencil.Stencil = 1.0f;

	api_ret_code = device->logical_device->CreateCommittedResource(
		&dsv_heap_prop,
		D3D12_HEAP_FLAG_NONE,
		&dsv_desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&swapchain->depth_stencil_clear_value,
		IID_PPV_ARGS(&swapchain->depth_stencil_resource));
	if (S_OK != api_ret_code)
	{
		HLEMER("failed to create depth stencil resource");
		return H_FAIL;
	}

	device->logical_device->CreateDepthStencilView(
		swapchain->depth_stencil_resource,
		&depth_stencil_view_desc,
		swapchain->dsv_heap->GetCPUDescriptorHandleForHeapStart());

	swapchain->viewport		= { 0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height), 0.0f, 1.0f };
	swapchain->scissor_rect = { 0, 0, width, height };

	return H_OK;
}

void clear_depth_stencil(directx_commandlist* commandlist, directx_swapchain* swapchain)
{
	commandlist->commandlist->ClearDepthStencilView(
		swapchain->dsv_heap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH,
		swapchain->depth_stencil_clear_value.DepthStencil.Depth,
		0,
		0,
		nullptr);
}

void bind_view_port(directx_commandlist* commandlist, directx_swapchain* swapchain)
{
	commandlist->commandlist->RSSetViewports(1, &swapchain->viewport);
}

void bind_scissor_rect(directx_commandlist* commandlist, directx_swapchain* swapchain)
{
	commandlist->commandlist->RSSetScissorRects(1, &swapchain->scissor_rect);
}

void set_view_port(directx_swapchain* swapchain, i32 left, i32 top, i32 right, i32 bottom)
{
	swapchain->scissor_rect = { left, top, right, bottom };
}

void set_view_port(directx_swapchain* swapchain, f32 width, f32 height, f32 topX, f32 topY)
{
	swapchain->viewport.Height	 = height;
	swapchain->viewport.Width	 = width;
	swapchain->viewport.TopLeftX = topX;
	swapchain->viewport.TopLeftY = topY;
	swapchain->viewport.MinDepth = 0.0f;
	swapchain->viewport.MaxDepth = 1.0f;
}

void set_depth_stencil(directx_swapchain* swapchain, f32 depth, u8 stencil)
{
	swapchain->depth_stencil_clear_value.DepthStencil.Depth	  = depth;
	swapchain->depth_stencil_clear_value.DepthStencil.Stencil = stencil;
}

i8 bind_rendertarget_and_depth_stencil(directx_commandlist* list, directx_swapchain* swapchain)
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = swapchain->dsv_heap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = swapchain->rtv_heap->GetCPUDescriptorHandleForHeapStart();

	rtv_handle.ptr += (swapchain->current_back_buffer_index * swapchain->rtv_heap_increment);

	list->commandlist->OMSetRenderTargets(1, &rtv_handle, true, &dsv_handle);

	return H_OK;
}

i8 swapchain_fail_handler(directx_swapchain* swapchain, swapchain_fails fail_code, u64 context)
{
	switch (fail_code)
	{
	case dsv_resource_creation_fail:
		swapchain->dsv_heap->Release();
	case dsv_heap_creation_fail:
		swapchain->rtv_heap->Release();
	case rtv_heap_creation_fail:
		for (int i = 0; i < context; i++)
		{
			swapchain->frame_resources[i]->Release();
		}
	case association_fail:
		swapchain->swapchain->Release();
	case swapchain_query_fail:
	case swapchain_fail:
		break;
	}
	return H_FAIL;
}
