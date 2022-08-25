#include "core/logger.hpp"
#include "debuglayer.hpp"
#include "renderer/backends/directx.hpp"

#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib") 

#include <stdlib.h>
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

i8 directx_initialize(renderer_backend* backend_ptr)
{
	i8 ret_code		= H_OK;
	
	HRESULT api_ret_code	= S_OK;

	HLINFO("Directx initialize");
	ENABLE_DEBUG_LAYER();	

	IDXGIFactory1* factory_1= nullptr;
	IDXGIFactory6* factory_6= nullptr; 
	IDXGIAdapter1* adapter	= nullptr;

	api_ret_code		= CreateDXGIFactory1(__uuidof(IDXGIFactory1),(void**)&factory_1);

	if( S_OK != api_ret_code )
	{
		HLEMER("Factory 1 dxgi creation failure");
		ret_code	= H_FAIL;
		goto fail;
	}

	api_ret_code 		= factory_1->QueryInterface(__uuidof(IDXGIFactory6),(void**)&factory_6);

	if( S_OK != api_ret_code )
	{
		HLEMER("dxgi factory 6 is not supported");
		ret_code	= H_FAIL;
		goto fail;
	}

	api_ret_code 		= factory_6->EnumAdapterByGpuPreference(0,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,__uuidof(IDXGIAdapter1),(void**)&adapter);
	
	if( S_OK != api_ret_code )
	{
		HLEMER("enumeration of adapter failure");
		ret_code	= H_FAIL;
		goto fail;
	}
	print_adapter_spec(adapter);

	api_ret_code		= D3D12CreateDevice(adapter,D3D_FEATURE_LEVEL_12_0,__uuidof(ID3D12Device),(void**)&device);
	if ( S_OK != api_ret_code )
	{
		HLEMER("device creation failure");
		ret_code	= H_FAIL;
		goto fail;
	}

	factory_1->Release();
	factory_6->Release();
	adapter->Release();
fail:
	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	device->Release();
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
