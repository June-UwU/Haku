/*****************************************************************//**
 * \file   helpers.hpp
 * \brief  generic helper functions
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "defines.hpp"
#include "directx_types.INL"


D3D12_ROOT_PARAMETER make_rootconstant(u32 reg, u32 space, u32 num_of_values, D3D12_SHADER_VISIBILITY visiblity);
					 
D3D12_ROOT_PARAMETER make_rootdescriptors(u32 reg, u32 space, D3D12_ROOT_PARAMETER_TYPE parameter_type,D3D12_SHADER_VISIBILITY visiblity);
					 
D3D12_ROOT_PARAMETER make_rootdescriptortable(u32 number_of_ranges, const D3D12_DESCRIPTOR_RANGE* ranges, D3D12_SHADER_VISIBILITY visiblity);

D3D12_DESCRIPTOR_RANGE make_range(D3D12_DESCRIPTOR_RANGE_TYPE type, u32 number_of_descriptors, u32 reg, u32 space, u32 offset);

D3D12_SHADER_BYTECODE make_shaderbytecode(ID3D10Blob* blob);

D3D12_RENDER_TARGET_BLEND_DESC make_rendertarget_blend_desc(bool blendenable, bool logicopenble, D3D12_BLEND src_blend, D3D12_BLEND dest_blend,
	D3D12_BLEND_OP blend_op, D3D12_BLEND src_blend_alpha, D3D12_BLEND dest_blend_alpha, D3D12_BLEND_OP blend_op_alpha, D3D12_LOGIC_OP logic_op, u8 write_mask);

D3D12_BLEND_DESC default_blend_desc();

D3D12_BLEND_DESC make_blenddesc(bool alpha_coverage, bool indpendant_blend, D3D12_RENDER_TARGET_BLEND_DESC rendertarget[8u]);

D3D12_DEPTH_STENCIL_DESC make_depth_stencil(bool depth_enable, D3D12_DEPTH_WRITE_MASK write_mask, D3D12_COMPARISON_FUNC comp_fun, bool stencil_enable,
	u8 stencil_read_mask, u8 stencil_write_mask, D3D12_DEPTH_STENCILOP_DESC frontface, D3D12_DEPTH_STENCILOP_DESC backface);

D3D12_RASTERIZER_DESC make_rasterize_desc(D3D12_FILL_MODE fill_mode, D3D12_CULL_MODE cull_mode, bool frontcc, i32 depthbias,
	f32 depthbiasclamp, f32 slope_scaled_depthbias, bool depthclip_enable, bool multisample_enable, bool aa_enable, 
	u32 forced_sample_count, D3D12_CONSERVATIVE_RASTERIZATION_MODE c_mode);

D3D12_GRAPHICS_PIPELINE_STATE_DESC make_pso_desc(ID3D12RootSignature* signature,
	ID3D10Blob* VS, ID3D10Blob* PS, ID3D10Blob* DS, ID3D10Blob* HS, ID3D10Blob* GS, D3D12_RASTERIZER_DESC raster_desc, D3D12_BLEND_DESC blend_desc,
	D3D12_DEPTH_STENCIL_DESC depth_stencil_state,DXGI_FORMAT rtv_format[8], u32 sample_mask, u32 RTVcount, u32 sample_count, u32 sample_quality);
