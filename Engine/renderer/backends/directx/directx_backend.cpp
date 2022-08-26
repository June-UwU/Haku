#include "directx.hpp"
#include "debuglayer.hpp"
#include "core/logger.hpp"
#include "directx_types.hpp"
#include "command_context.hpp"

#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib") 

#include <D3d12.h>
#include <dxgi1_6.h>


static ID3D12Device* device;

// helper function that prints adapter features
void print_adapter_spec(IDXGIAdapter1* adapter)
{
	DXGI_ADAPTER_DESC1 desc;
	adapter->GetDesc1(&desc);
	char gpu_char[128];
	HLINFO("Direct GPU detection");
	HLINFO("GPU		: %ls",desc.Description);
}

// helper function to create device
i8 create_device(void)
{
	HLINFO("device creation");
	i8 ret_code		= H_OK;

	HRESULT api_ret_code	= S_OK;

	IDXGIFactory1* factory_1 = nullptr;
	IDXGIFactory6* factory_6 = nullptr;
	IDXGIAdapter1* adapter	 = nullptr;

	api_ret_code = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory_1);

	if (S_OK != api_ret_code)
	{
		HLEMER("Factory 1 dxgi creation failure");
		ret_code = H_FAIL;
		goto fail_factory_1;
	}

	api_ret_code = factory_1->QueryInterface(__uuidof(IDXGIFactory6), (void**)&factory_6);

	if (S_OK != api_ret_code)
	{
		HLEMER("dxgi factory 6 is not supported");
		ret_code = H_FAIL;
		goto fail_factory_6;
	}

	api_ret_code = factory_6->EnumAdapterByGpuPreference(
		0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter1), (void**)&adapter);

	if (S_OK != api_ret_code)
	{
		HLEMER("enumeration of adapter failure");
		ret_code = H_FAIL;
		goto fail_adapter;
	}
	print_adapter_spec(adapter);

	api_ret_code = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&device);
	if (S_OK != api_ret_code)
	{
		HLEMER("device creation failure");
		ret_code = H_FAIL;
		goto fail_device;
	}

	FRIENDLY_NAME(device, L"DirectX device");

fail_device:
	factory_6->Release();
fail_adapter:
	adapter->Release();
fail_factory_6:
	factory_1->Release();
fail_factory_1:
	return ret_code;
}

void device_destroy(void)
{
	HLINFO("device shutdown");
	device->Release();
}

i8 directx_initialize(renderer_backend* backend_ptr)
{
	i8 ret_code		= H_OK;
	
	HLINFO("Directx initialize");
	ENABLE_DEBUG_LAYER();	
	
	ret_code = create_device();
	if (H_FAIL == ret_code)
	{
		goto init_fail;
	}

	ret_code = command_context_initialize(device);
	if (H_FAIL == ret_code)
	{
		goto command_fail;
	}
	goto h_ok;



command_fail:
	device_destroy();
init_fail:
h_ok:
	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	command_context_shutdown();
	device_destroy();
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
