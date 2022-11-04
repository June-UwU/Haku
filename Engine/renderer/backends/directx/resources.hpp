/*****************************************************************//**
 * \file   resources.hpp
 * \brief  directx resources
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "defines.hpp"
#include "directx_types.INL"



ID3D12Resource* create_resource(u64 height, u64 width, u32 depth, D3D12_RESOURCE_DIMENSION dimension, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS h_flags
    , DXGI_FORMAT  format, D3D12_RESOURCE_STATES  state, D3D12_RESOURCE_FLAGS resource_flags);


void destroy_resource(ID3D12Resource* resource);