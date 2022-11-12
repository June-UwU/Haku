/*****************************************************************//**
 * \file   copyengine.hpp
 * \brief  haku copy engine 
 * 
 * \author June
 * \date   November 2022
 *********************************************************************/
#pragma once
#include "defines.hpp"
#include "../directx_types.INL"

i8 initialize_copy_engine(void);

void shutdown_copy_engine(void);

void copy_buffer(ID3D12Resource* dest, void* data, u64 height, u64 width, u64 depth, D3D12_RESOURCE_DIMENSION dimension);

i8 do_copy(void);

void transition_buffer_immediate(ID3D12Resource* dest, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, u64 subresource);
