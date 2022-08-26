#include "core/logger.hpp"
#include "command_context.hpp"

static ID3D12CommandQueue* directx_queue[HK_COMMAND_MAX];

i8 command_context_initialize(ID3D12Device* device)
{
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
	FRIENDLY_NAME(device, L"DirectX Copy Queue");
	goto h_ok;


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
}

void execute_command(queue_type type)
{

}
