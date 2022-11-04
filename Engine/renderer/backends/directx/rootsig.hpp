/*****************************************************************//**
 * \file   rootsig.hpp
 * \brief  root signature creation 
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "defines.hpp"
#include "directx_types.INL"
#include "helpers.hpp"


ID3D12RootSignature* create_rootsignature(u32 number_of_rparam, D3D12_ROOT_PARAMETER* param, u32 number_of_sampler, 
	D3D12_STATIC_SAMPLER_DESC* samplers, D3D12_ROOT_SIGNATURE_FLAGS flags);


void destroy_rootsignature(ID3D12RootSignature* signature);
