#include "directx.hpp"
#include "debuglayer.hpp"
#include "core/logger.hpp"
#include "command_context.hpp"
#include "swapchain.hpp"

#include "directx_types.inl"


#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib") 

typedef enum
{
	swapchain_fail,
	command_fail,
	fail_device,
	fail_adapter,
	fail_factory_6
} context_fails;

i8 context_fail_handler(directx_context* context, context_fails fail_code);

static directx_context	context;
	
// helper function that prints adapter features
void print_adapter_spec(IDXGIAdapter1* adapter)
{
	DXGI_ADAPTER_DESC1 desc;
	adapter->GetDesc1(&desc);
	char gpu_char[128];
	HLINFO("Direct GPU detection");
	HLINFO("GPU		: %ls",desc.Description);
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
	i8 ret_code		= H_OK;

	HRESULT api_ret_code	= S_OK;

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
	
	factory_1->Release();
	return ret_code;
}

i8 directx_initialize(renderer_backend* backend_ptr)
{
	i8 ret_code		= H_OK;
	
	HLINFO("Directx initialize");
	ENABLE_DEBUG_LAYER();	
	
	ret_code = create_context();
	if (H_FAIL == ret_code)
	{
		goto init_fail;
	}
	goto h_ok;

init_fail:
h_ok:
	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	context_destroy();
	command_context_shutdown(&context.queue);
	swapchain_shutdown(&context.swapchain);
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

i8 context_fail_handler(directx_context* context, context_fails fail_code)
{
	switch (fail_code)
	{
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
