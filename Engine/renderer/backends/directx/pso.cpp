/*****************************************************************//**
 * \file   pso.cpp
 * \brief  
 * 
 * \author June
 * \date   November 2022
 *********************************************************************/
#include "pso.hpp"
#include "directx_backend.hpp"
#include "core/logger.hpp"

ID3D12PipelineState* create_pipelinestate(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc)
{
	ID3D12PipelineState* rv = nullptr;
	directx_device* dev = get_device();

	HRESULT win32_rv = dev->logical_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&rv));
	if (S_OK != win32_rv)
	{
		HLEMER("failed to create pipeline state");
		return nullptr;
	}
	return rv;
}

void destroy_pipelinestate(ID3D12PipelineState* state)
{
	state->Release();
}
