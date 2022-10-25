/*****************************************************************/ /**
* \file   pipeline_state.cpp
* \brief  pipeline state implementations
*
* \author June
* \date   September 2022
*********************************************************************/

#include "pipeline_state.hpp"
#include "core/logger.hpp"
#include "memory/hmemory.hpp"
#include "rootsignature.hpp"
// TODO : remove creation of root signature from here

directx_pipeline* create_pipeline_state(directx_device* device, directx_shader_module* module)
{
	directx_pipeline* ret_ptr = (directx_pipeline*)hmemory_alloc(sizeof(directx_pipeline), MEM_TAG_RENDERER);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc{};
	if (nullptr != module->compiled_shaders[HK_VERTEX_SHADER])
	{
		D3D12_SHADER_BYTECODE vertex_bytecode{};
		vertex_bytecode.BytecodeLength	= module->compiled_shaders[HK_VERTEX_SHADER]->GetBufferSize();
		vertex_bytecode.pShaderBytecode = module->compiled_shaders[HK_VERTEX_SHADER]->GetBufferPointer();
		pipeline_desc.VS				= vertex_bytecode;
	}

	if (nullptr != module->compiled_shaders[HK_HULL_SHADER])
	{
		D3D12_SHADER_BYTECODE hull_bytecode{};
		hull_bytecode.BytecodeLength  = module->compiled_shaders[HK_HULL_SHADER]->GetBufferSize();
		hull_bytecode.pShaderBytecode = module->compiled_shaders[HK_HULL_SHADER]->GetBufferPointer();
		pipeline_desc.HS			  = hull_bytecode;
	}

	if (nullptr != module->compiled_shaders[HK_DOMAIN_SHADER])
	{
		D3D12_SHADER_BYTECODE domain_bytecode{};
		domain_bytecode.BytecodeLength	= module->compiled_shaders[HK_DOMAIN_SHADER]->GetBufferSize();
		domain_bytecode.pShaderBytecode = module->compiled_shaders[HK_DOMAIN_SHADER]->GetBufferPointer();
		pipeline_desc.DS				= domain_bytecode;
	}

	if (nullptr != module->compiled_shaders[HK_GEOMETRY_SHADER])
	{
		D3D12_SHADER_BYTECODE geometry_bytecode{};
		geometry_bytecode.BytecodeLength  = module->compiled_shaders[HK_GEOMETRY_SHADER]->GetBufferSize();
		geometry_bytecode.pShaderBytecode = module->compiled_shaders[HK_GEOMETRY_SHADER]->GetBufferPointer();
		pipeline_desc.GS				  = geometry_bytecode;
	}

	if (nullptr != module->compiled_shaders[HK_PIXEL_SHADER])
	{
		D3D12_SHADER_BYTECODE pixel_bytecode{};
		pixel_bytecode.BytecodeLength  = module->compiled_shaders[HK_PIXEL_SHADER]->GetBufferSize();
		pixel_bytecode.pShaderBytecode = module->compiled_shaders[HK_PIXEL_SHADER]->GetBufferPointer();
		pipeline_desc.PS			   = pixel_bytecode;
	}

	// this is the default blend state for the moment
	D3D12_BLEND_DESC blend_state{};
	blend_state.AlphaToCoverageEnable				  = false;
	blend_state.IndependentBlendEnable				  = false;
	blend_state.RenderTarget[0].BlendEnable			  = false;
	blend_state.RenderTarget[0].LogicOpEnable		  = false;
	blend_state.RenderTarget[0].SrcBlend			  = D3D12_BLEND_ONE;
	blend_state.RenderTarget[0].DestBlend			  = D3D12_BLEND_ZERO;
	blend_state.RenderTarget[0].BlendOp				  = D3D12_BLEND_OP_ADD;
	blend_state.RenderTarget[0].SrcBlendAlpha		  = D3D12_BLEND_ONE;
	blend_state.RenderTarget[0].DestBlendAlpha		  = D3D12_BLEND_ZERO;
	blend_state.RenderTarget[0].BlendOpAlpha		  = D3D12_BLEND_OP_ADD;
	blend_state.RenderTarget[0].LogicOp				  = D3D12_LOGIC_OP_NOOP;
	blend_state.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// default raster desc
	D3D12_RASTERIZER_DESC raster_desc{};

	raster_desc.FillMode			  = D3D12_FILL_MODE_SOLID;
	raster_desc.CullMode			  = D3D12_CULL_MODE_BACK;
	raster_desc.FrontCounterClockwise = FALSE;
	raster_desc.DepthBias			  = D3D12_DEFAULT_DEPTH_BIAS;
	raster_desc.DepthBiasClamp		  = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	raster_desc.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	raster_desc.DepthClipEnable		  = TRUE;
	raster_desc.MultisampleEnable	  = FALSE;
	raster_desc.AntialiasedLineEnable = FALSE;
	raster_desc.ForcedSampleCount	  = 0;
	raster_desc.ConservativeRaster	  = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// default depth stencil desc
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = {
		D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS
	};
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable		= TRUE;
	depth_stencil_desc.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc		= D3D12_COMPARISON_FUNC_LESS;
	depth_stencil_desc.StencilEnable	= FALSE;
	depth_stencil_desc.StencilReadMask	= D3D12_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.FrontFace		= defaultStencilOp;
	depth_stencil_desc.BackFace			= defaultStencilOp;

	constexpr const D3D12_INPUT_ELEMENT_DESC InputElementDesc[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputdesc{ InputElementDesc, 2 };

	// TODO : remove creation of root signature from here
	CREATE_PSO_ROOT_SIGNATURE(ret_ptr);
	create_root_signature(device, ret_ptr->signature);

	pipeline_desc.pRootSignature				  = ret_ptr->signature->root_signature;
	pipeline_desc.BlendState					  = blend_state;
	pipeline_desc.SampleMask					  = UINT_MAX;
	pipeline_desc.RasterizerState				  = raster_desc;
	pipeline_desc.DepthStencilState				  = depth_stencil_desc;
	pipeline_desc.InputLayout					  = inputdesc;
	pipeline_desc.IBStripCutValue				  = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pipeline_desc.PrimitiveTopologyType			  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipeline_desc.NumRenderTargets				  = 1;
	pipeline_desc.RTVFormats[0]					  = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipeline_desc.DSVFormat						  = DXGI_FORMAT_D32_FLOAT;
	pipeline_desc.SampleDesc.Count				  = 1;
	pipeline_desc.SampleDesc.Quality			  = 0;
	pipeline_desc.CachedPSO.pCachedBlob			  = NULL;
	pipeline_desc.CachedPSO.CachedBlobSizeInBytes = 0;
	pipeline_desc.Flags							  = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT api_ret_val = device->logical_device->CreateGraphicsPipelineState(
		&pipeline_desc, __uuidof(ID3D12PipelineState), (void**)&ret_ptr->state);

	if (S_OK != api_ret_val)
	{
		HLCRIT("failed to create pipeline state");
		hmemory_free(ret_ptr, MEM_TAG_RENDERER);
		return nullptr;
	}

	return ret_ptr;
}

void destroy_pipeline_state(directx_pipeline* pipeline)
{
	pipeline->state->Release();
	destroy_root_signature(pipeline->signature);
	destroy_darray(pipeline->signature->parameter_array);
	destroy_darray(pipeline->signature->sampler_array);
	hmemory_free(pipeline->signature, MEM_TAG_RENDERER);
	hmemory_free(pipeline, MEM_TAG_RENDERER);
}

void bind_pipeline_state(directx_cc* commandlist, directx_pipeline* pipeline)
{
	bind_root_signature(commandlist, pipeline->signature);
	commandlist->commandlist->SetPipelineState(pipeline->state);
}
