#include "core/logger.hpp"
#include "command_context.hpp"

#include <platform/platform.hpp>

static ID3D12CommandQueue* directx_queue[HK_COMMAND_MAX];
static IDXGISwapChain* swapchain;	// directx swap chain

i8 command_context_initialize(ID3D12Device* device, IDXGIFactory6* factory_6)
{

// initialization of nessary data
	// getting win32 platform specfic data
	void* handle;
	platform_data_for_render_api(&handle);

	p_prop plat_prop;
	get_platform_properties(&plat_prop);

	DXGI_SWAP_CHAIN_DESC sdesc{};
	// swapa chain buffer desc
	sdesc.BufferDesc.Width = plat_prop.width;
	sdesc.BufferDesc.Height = plat_prop.height;
	sdesc.BufferCount = 3u;
	sdesc.BufferDesc.RefreshRate.Numerator = 0u;
	sdesc.BufferDesc.RefreshRate.Denominator = 0u;
	sdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// multi sampling conf , for now MULTI SAMPLING IS OFF
	sdesc.SampleDesc.Count = 1;
	sdesc.SampleDesc.Quality = 0;

	// buffer usage
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// swap effect
	sdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sdesc.Windowed = TRUE;
	sdesc.OutputWindow = *(HWND*)handle;


// End of initialization for variables


	HLINFO("command queue initialization");
	i8 ret_code		= H_OK;
	HRESULT	api_ret_code	= S_OK;

	D3D12_COMMAND_QUEUE_DESC q_desc{};
	q_desc.Type	= D3D12_COMMAND_LIST_TYPE_DIRECT;
	q_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	q_desc.Flags	= D3D12_COMMAND_QUEUE_FLAG_NONE;
	q_desc.NodeMask	= 0;

	// creating render queue
	api_ret_code	= device->CreateCommandQueue(&q_desc,__uuidof(ID3D12CommandQueue),(void**)&directx_queue[HK_COMMAND_RENDER]);
	if(S_OK != api_ret_code)
	{
		ret_code= H_FAIL;
		HLEMER("render queue failure");
		goto render_queue_fail;
	}
	HLINFO("render queue initailized");
	FRIENDLY_NAME(device, L"DirectX Render Queue");



	q_desc.Type	= D3D12_COMMAND_LIST_TYPE_COMPUTE;

	// creating compute queue
	api_ret_code	= device->CreateCommandQueue(&q_desc,__uuidof(ID3D12CommandQueue),(void**)&directx_queue[HK_COMMAND_COMPUTE]);
	if(S_OK != api_ret_code)
	{
		ret_code= H_FAIL;
		HLEMER("compute queue creation failure");
		goto compute_queue_fail;
	}
	HLINFO("compute queue initailized");
	FRIENDLY_NAME(device, L"DirectX Compute Queue");

	q_desc.Type	= D3D12_COMMAND_LIST_TYPE_COPY;

	// creating copy queue
	api_ret_code	= device->CreateCommandQueue(&q_desc,__uuidof(ID3D12CommandQueue),(void**)&directx_queue[HK_COMMAND_COPY]);
	if(S_OK != api_ret_code)
	{
		ret_code= H_FAIL;
		HLEMER("copy queue failure");
		goto copy_queue_failure;
	}
	HLINFO("copy queue initailized");
	FRIENDLY_NAME(device, L"DirectX Copy Queue");
	


	api_ret_code = factory_6->CreateSwapChain(directx_queue[HK_COMMAND_RENDER], &sdesc, &swapchain);
	if (S_OK != api_ret_code)
	{
		HLEMER("swapchain creation failure");
		ret_code = H_FAIL;
		goto swapchain_fail;
	}
	HLINFO("swap chain initailized");

	api_ret_code = factory_6->MakeWindowAssociation(*(HWND*)handle, DXGI_MWA_NO_WINDOW_CHANGES);
	if (S_OK != api_ret_code)
	{
		HLEMER("window assciation failure");
		ret_code = H_FAIL;
		goto association_fail;
	}
	HLINFO("default behavior set");

	goto h_ok;



association_fail:
	swapchain->Release();
swapchain_fail:
	directx_queue[HK_COMMAND_COPY]->Release();
copy_queue_failure:
	directx_queue[HK_COMMAND_COMPUTE]->Release();
compute_queue_fail:
	directx_queue[HK_COMMAND_RENDER]->Release();
render_queue_fail:
h_ok:
	return ret_code;
}

void command_context_shutdown(void)
{
	HLINFO("command queue shutdown");
	for(u64 i = 0; i< HK_COMMAND_MAX; i++)
	{
		directx_queue[i]->Release();
	}
	swapchain->Release();
}

void execute_command(queue_type type)
{

}
