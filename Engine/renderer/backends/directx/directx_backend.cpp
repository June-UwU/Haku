#include "directx.hpp"
#include "debuglayer.hpp"
#include "core/logger.hpp"
#include "command_context.hpp"
#include "swapchain.hpp"
#include "command_buffer_pool.hpp"

#include "directx_types.inl"


#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib") 

typedef enum
{
	commandlist_fail,
	command_allocator_fail,
	swapchain_fail,
	command_fail,
	fail_device,
	fail_adapter,
	fail_factory_6
} context_fails;

typedef enum
{
	render_commandlist_fail,
	copy_commandlist_fail,
	compute_commandlist_fail,
	clean_commandlist_fail
} commandlist_fails;

// internal functions that are used to sent up context and help with other things
i8 create_context(void); // context creator
i8 create_commandlist(void); // command list creator routine
void context_destroy(void);  // context destroy routine
void commandlist_destroy(void);// command list destroy routine
void print_adapter_spec(IDXGIAdapter1* adapter); // print gpu specs 
i8 commandlist_fail_handler(commandlist_fails fail_code); // command list fail handler since labels and gotos push me to c99 and I don't wike it
i8 context_fail_handler(directx_context* context, context_fails fail_code);// context fail handler

// one static instance of directx context 
static directx_context	context;
	
i8 directx_initialize(renderer_backend* backend_ptr)
{
	i8 ret_code		= H_OK;
	
	HLINFO("Directx initialize");
	ENABLE_DEBUG_LAYER();	
	
	ret_code = create_context();
	
	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	context_destroy();
	command_context_shutdown(&context.queue);
	swapchain_shutdown(&context.swapchain);
	command_buffer_pool_shutdown();
	commandlist_destroy();
}

i8 directx_begin_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code	= H_OK;
	

	return ret_code;
}

i8 directx_end_frame(renderer_backend* backend_ptr,f64 delta_time)
{
	i8 ret_code	= H_OK;


	return ret_code;
}


// helper function to destroy device 
void context_destroy(void)
{
	HLINFO("context shutdown");
	context.logical_device->Release();
	context.physical_device->Release();
	context.factory->Release();
}


// helper function to create device and swap chain
i8 create_context(void)
{
	HLINFO("device creation");
	i8 ret_code = H_OK;

	HRESULT api_ret_code = S_OK;

	IDXGIFactory1* factory_1 = nullptr;


	api_ret_code = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory_1);

	if (S_OK != api_ret_code)
	{
		HLEMER("Factory 1 dxgi creation failure");
		ret_code = H_FAIL;
		return ret_code;
	}

	api_ret_code = factory_1->QueryInterface(__uuidof(IDXGIFactory6), (void**)&context.factory);

	if (S_OK != api_ret_code)
	{
		HLEMER("dxgi factory 6 is not supported");
		factory_1->Release();
		ret_code = context_fail_handler(&context, fail_factory_6);
		return ret_code;
	}

	api_ret_code = context.factory->EnumAdapterByGpuPreference(
		0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter1), (void**)&context.physical_device);

	if (S_OK != api_ret_code)
	{
		HLEMER("enumeration of adapter failure");
		factory_1->Release();
		ret_code = context_fail_handler(&context, fail_adapter);
		return ret_code;
	}
	print_adapter_spec(context.physical_device);

	api_ret_code = D3D12CreateDevice(context.physical_device, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&context.logical_device);
	if (S_OK != api_ret_code)
	{
		HLEMER("device creation failure");
		factory_1->Release();
		ret_code = context_fail_handler(&context, fail_device);
		return ret_code;
	}
	HLINFO("Device initailized");
	FRIENDLY_NAME(context.logical_device, L"DirectX device");

	ret_code = command_context_initialize(&context);
	if (H_FAIL == ret_code)
	{
		factory_1->Release();
		ret_code = context_fail_handler(&context, command_fail);
		return ret_code;
	}

	ret_code = swapchain_initialize(&context);
	if (H_FAIL == ret_code)
	{
		factory_1->Release();
		ret_code = context_fail_handler(&context, swapchain_fail);
		return ret_code;
	}

	ret_code = command_buffer_pool(&context, command_allocator_size);
	if (H_FAIL == ret_code)
	{
		factory_1->Release();
		ret_code = context_fail_handler(&context, command_allocator_fail);
		return ret_code;
	}

	ret_code = create_commandlist();
	if (H_FAIL == ret_code)
	{
		factory_1->Release();
		ret_code = context_fail_handler(&context, commandlist_fail);
		return ret_code;
	}

	factory_1->Release();
	return ret_code;
}

// helper to create commandlists in context
i8 create_commandlist(void)
{
	i8 ret_code = H_OK;
	HRESULT api_ret_code  = S_OK;

	directx_commandlist* commandlist = context.commandlist;
	directx_allocator* allocator = request_command_buffer(HK_COMMAND_RENDER);


	// render command list create
	api_ret_code = context.logical_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator->allocator, nullptr,
		__uuidof(ID3D12GraphicsCommandList), (void**)&commandlist[HK_COMMAND_RENDER].commandlist);
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist creation failed");
		ret_code = commandlist_fail_handler(render_commandlist_fail);
		return ret_code;
	}
	api_ret_code = commandlist[HK_COMMAND_RENDER].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist close failed");
		ret_code = commandlist_fail_handler(copy_commandlist_fail);
		return ret_code;
	}
	commandlist[HK_COMMAND_RENDER].state = COMMANDLIST_STALE;
	return_directx_allocator(allocator);

	// copy list create
	allocator = request_command_buffer(HK_COMMAND_COPY);
	api_ret_code = context.logical_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, allocator->allocator, nullptr,
		__uuidof(ID3D12GraphicsCommandList), (void**)&commandlist[HK_COMMAND_COPY].commandlist);
	if (S_OK != api_ret_code)
	{
		HLEMER("copy commandlist creation failed");
		ret_code = commandlist_fail_handler(copy_commandlist_fail);
		return ret_code;
	}
	api_ret_code = commandlist[HK_COMMAND_COPY].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist close failed");
		ret_code = commandlist_fail_handler(compute_commandlist_fail);
		return ret_code;
	}
	commandlist[HK_COMMAND_COPY].state = COMMANDLIST_STALE;
	return_directx_allocator(allocator);

	// compute list create
	allocator = request_command_buffer(HK_COMMAND_COMPUTE);
	api_ret_code = context.logical_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, allocator->allocator, nullptr,
		__uuidof(ID3D12GraphicsCommandList), (void**)&commandlist[HK_COMMAND_COMPUTE].commandlist);
	if (S_OK != api_ret_code)
	{
		HLEMER("compute commandlist creation failed");
		ret_code = commandlist_fail_handler(compute_commandlist_fail);
		return ret_code;
	}
	api_ret_code = commandlist[HK_COMMAND_COMPUTE].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist close failed");
		ret_code = commandlist_fail_handler(clean_commandlist_fail);
		return ret_code;
	}
	commandlist[HK_COMMAND_COMPUTE].state = COMMANDLIST_STALE;
	return_directx_allocator(allocator);

	return ret_code;
}

// helper to clean commandlists
void commandlist_destroy(void)
{
	HLINFO("command list shutdown");
	directx_commandlist* commandlist = context.commandlist;
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		commandlist[i].commandlist->Release();
	}
}

// FAIL HANDLERS

i8 commandlist_fail_handler(commandlist_fails fail_code)
{
	directx_commandlist* commandlist = context.commandlist;
	switch (fail_code)
	{
	case clean_commandlist_fail:
		commandlist[HK_COMMAND_COMPUTE].commandlist->Release();
	case compute_commandlist_fail:
		commandlist[HK_COMMAND_COPY].commandlist->Release();
	case copy_commandlist_fail:
		commandlist[HK_COMMAND_RENDER].commandlist->Release();
	case render_commandlist_fail:
		break;
	}
	return H_FAIL;
}


i8 context_fail_handler(directx_context* context, context_fails fail_code)
{
	switch (fail_code)
	{
	case commandlist_fail:
		command_buffer_pool_shutdown();
	case command_allocator_fail:
		swapchain_shutdown(&context->swapchain);
	case swapchain_fail:
		command_context_shutdown(&context->queue);
	case command_fail:
		context_destroy();
	case fail_device:
		context->factory->Release();
	case fail_adapter:
		context->physical_device->Release();
	case fail_factory_6:
		break;
	}

	return H_FAIL;
}


// MISC

// helper function that prints adapter features
void print_adapter_spec(IDXGIAdapter1* adapter)
{
	DXGI_ADAPTER_DESC1 desc;
	adapter->GetDesc1(&desc);
	char gpu_char[128];
	HLINFO("Direct GPU detection");
	HLINFO("GPU		: %ls", desc.Description);
}
