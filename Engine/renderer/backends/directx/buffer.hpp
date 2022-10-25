/*****************************************************************//**
 * \file   buffer.hpp
 * \brief  directx buffer impl
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include "directx_types.INL"

// TODO : init, shutdown , create , copy , resize, and load_data

i8 initialize_buffer_structures(void);

void shutdown_buffer_structure();


i8 create_buffer(directx_device* device, directx_buffer* out_buffer, u64 width, u64 height, u64 depth, D3D12_RESOURCE_DIMENSION dimension);

void release_buffer(directx_buffer* buffer);

void transition_buffer(directx_buffer* buffer, u64 sub_resource, D3D12_RESOURCE_STATES after_state);

void copy_buffer(directx_buffer* dest, directx_buffer* src);

void resize_buffer(directx_buffer* buffer);

void load_buffer(directx_buffer* buffer, void* data, u64 size, u64 sub_resource);

void reclaim_buffer(u64 fence_val);