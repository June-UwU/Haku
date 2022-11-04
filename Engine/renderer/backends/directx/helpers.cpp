/*****************************************************************//**
 * \file   helpers.cpp
 * \brief  
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "helpers.hpp"
#include "core/logger.hpp"

D3D12_ROOT_PARAMETER make_rootconstant(u32 reg, u32 space, u32 num_of_values, D3D12_SHADER_VISIBILITY visiblity)
{
	D3D12_ROOT_PARAMETER rv{};

	rv.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rv.Constants.Num32BitValues = num_of_values;
	rv.Constants.RegisterSpace = space;
	rv.Constants.ShaderRegister = reg;
	rv.ShaderVisibility = visiblity;

	return rv;
}

D3D12_ROOT_PARAMETER make_rootdescriptors(u32 reg, u32 space, D3D12_ROOT_PARAMETER_TYPE parameter_type, D3D12_SHADER_VISIBILITY visiblity)
{
	D3D12_ROOT_PARAMETER rv{};

	rv.ParameterType = parameter_type;
	rv.Descriptor.RegisterSpace = space;
	rv.Descriptor.ShaderRegister = reg;
	rv.ShaderVisibility = visiblity;

	return rv;
}

D3D12_ROOT_PARAMETER make_rootdescriptortable(u32 number_of_ranges, const D3D12_DESCRIPTOR_RANGE* ranges, D3D12_SHADER_VISIBILITY visiblity)
{
	D3D12_ROOT_PARAMETER rv{};

	rv.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rv.DescriptorTable.NumDescriptorRanges = number_of_ranges;
	rv.DescriptorTable.pDescriptorRanges = ranges;
	rv.ShaderVisibility = visiblity;

	return rv;
}

D3D12_DESCRIPTOR_RANGE make_range(D3D12_DESCRIPTOR_RANGE_TYPE type, u32 number_of_descriptors, u32 reg, u32 space, u32 offset)
{
	D3D12_DESCRIPTOR_RANGE rv;

	rv.RangeType = type;
	rv.NumDescriptors = number_of_descriptors;
	rv.OffsetInDescriptorsFromTableStart = offset;
	rv.BaseShaderRegister = reg;
	rv.RegisterSpace = space;

	return rv;
}

D3D12_SHADER_BYTECODE make_shaderbytecode(ID3D10Blob* blob)
{
	D3D12_SHADER_BYTECODE rv{};

	rv.BytecodeLength = blob->GetBufferSize();
	rv.pShaderBytecode = blob->GetBufferPointer();

	return rv;
}



D3D12_RENDER_TARGET_BLEND_DESC make_rendertarget_blend_desc(bool blendenable, bool logicopenble, D3D12_BLEND src_blend, D3D12_BLEND dest_blend, D3D12_BLEND_OP blend_op, D3D12_BLEND src_blend_alpha, D3D12_BLEND dest_blend_alpha, D3D12_BLEND_OP blend_op_alpha, D3D12_LOGIC_OP logic_op, u8 write_mask)
{
	D3D12_RENDER_TARGET_BLEND_DESC rv{};

	rv.BlendEnable = blendenable;
	rv.LogicOpEnable = logicopenble;
	rv.SrcBlend = src_blend;
	rv.DestBlend = dest_blend;
	rv.BlendOp = blend_op;
	rv.SrcBlendAlpha = src_blend_alpha;
	rv.DestBlendAlpha = dest_blend_alpha;
	rv.BlendOpAlpha = blend_op_alpha;
	rv.LogicOp = logic_op;
	rv.RenderTargetWriteMask = write_mask;

	return rv;
}

D3D12_BLEND_DESC default_blend_desc()
{
	D3D12_BLEND_DESC rv{};
	
	rv.AlphaToCoverageEnable = FALSE;
	rv.IndependentBlendEnable = FALSE;
	rv.RenderTarget[0].BlendEnable = FALSE;
	rv.RenderTarget[0].LogicOpEnable = FALSE;
	rv.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	rv.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	rv.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	rv.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	rv.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	rv.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rv.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	rv.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	
	return rv;
}

D3D12_BLEND_DESC make_blenddesc(bool alpha_coverage, bool indpendant_blend, D3D12_RENDER_TARGET_BLEND_DESC rendertarget[8u])
{
	D3D12_BLEND_DESC rv{};

	rv.AlphaToCoverageEnable = alpha_coverage;
	rv.IndependentBlendEnable = indpendant_blend;
	for (u8 i = 0; i < 8u; i++)
	{
		rv.RenderTarget[i] = rendertarget[i];
	}

	return rv;
}

D3D12_DEPTH_STENCIL_DESC make_depth_stencil(bool depth_enable, D3D12_DEPTH_WRITE_MASK write_mask, D3D12_COMPARISON_FUNC comp_fun, bool stencil_enable, u8 stencil_read_mask, u8 stencil_write_mask, D3D12_DEPTH_STENCILOP_DESC frontface, D3D12_DEPTH_STENCILOP_DESC backface)
{
	D3D12_DEPTH_STENCIL_DESC rv{};

	rv.DepthEnable = depth_enable;
	rv.DepthWriteMask = write_mask;
	rv.DepthFunc = comp_fun;
	rv.StencilEnable = stencil_enable;
	rv.StencilReadMask = stencil_read_mask;
	rv.StencilWriteMask = stencil_write_mask;
	rv.FrontFace = frontface;
	rv.BackFace = backface;

	return rv;
}

D3D12_RASTERIZER_DESC make_rasterize_desc(D3D12_FILL_MODE fill_mode, D3D12_CULL_MODE cull_mode, bool frontcc, i32 depthbias, f32 depthbiasclamp, f32 slope_scaled_depthbias, bool depthclip_enable, bool multisample_enable, bool aa_enable, u32 forced_sample_count, D3D12_CONSERVATIVE_RASTERIZATION_MODE c_mode)
{
	D3D12_RASTERIZER_DESC rv;

	rv.FillMode = fill_mode;
	rv.CullMode = cull_mode;
	rv.FrontCounterClockwise = frontcc;
	rv.DepthBias = depthbias;
	rv.DepthBiasClamp = depthbiasclamp;
	rv.SlopeScaledDepthBias = slope_scaled_depthbias;
	rv.DepthClipEnable = depthclip_enable;
	rv.MultisampleEnable = multisample_enable;
	rv.AntialiasedLineEnable = aa_enable;
	rv.ForcedSampleCount = forced_sample_count;
	rv.ConservativeRaster = c_mode;

	return rv;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC make_pso_desc(ID3D12RootSignature* signature,
	ID3D10Blob* VS, ID3D10Blob* PS, ID3D10Blob* DS, ID3D10Blob* HS, ID3D10Blob* GS, D3D12_RASTERIZER_DESC raster_desc, D3D12_BLEND_DESC blend_desc,
	D3D12_DEPTH_STENCIL_DESC depth_stencil_state, DXGI_FORMAT rtv_format[8], u32 sample_mask, u32 RTVcount, u32 sample_count, u32 sample_quality)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC rv{};

	rv.pRootSignature = signature;
	if (nullptr != VS)
	{
		rv.VS = make_shaderbytecode(VS);
	}
	else
	{
		HLEMER("no vertex shader set");
	}

	if (nullptr != PS)
	{
		rv.PS = make_shaderbytecode(PS);
	}

	if (nullptr != DS)
	{
		rv.DS = make_shaderbytecode(DS);
	}

	if (nullptr != HS)
	{
		rv.HS = make_shaderbytecode(HS);
	}

	if (nullptr != GS)
	{
		rv.GS = make_shaderbytecode(GS);

	}

	rv.RasterizerState = raster_desc;
	rv.BlendState = blend_desc;
	rv.DepthStencilState = depth_stencil_state;
	rv.SampleMask = sample_mask;
	rv.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	rv.NumRenderTargets = RTVcount;
	rv.SampleDesc.Count = sample_count;
	rv.SampleDesc.Quality = sample_quality;
	rv.InputLayout = INPUT_LAYOUT_DESC;
	rv.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	for (u8 i = 0; i < 8u; i++)
	{
		rv.RTVFormats[i] = rtv_format[i];
	}

	return rv;

}
