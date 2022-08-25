#include "debuglayer.hpp"
#include "core/logger.hpp"

// TODO : make this generate based on platforms and apis used

#include <d3d12.h>

static ID3D12Debug* debug_interface;

i8 debug_layer_initialize(void)
{
	HRESULT ret_code;
	ret_code	= D3D12GetDebugInterface(__uuidof(ID3D12Debug),(void**)&debug_interface);
	if(S_OK	== ret_code)
	{
		debug_interface->EnableDebugLayer();
		HLINFO("DirectX debugg layer enable");
	}
	return H_OK;
}

void debug_layer_shutdown(void)
{
	debug_interface->Release();
}
