/*****************************************************************//**
 * \file   gpu_descriptors.hpp
 * \brief  haku directx descriptor heap management files
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include "directx_types.INL"

// FIX ME  : this currently runs on hopes and dreams and is should be rewritten with some logic that handles the frames

constexpr const u32 SAMPLER_CAP = 2048u; // 
constexpr const u32 RESOURCE_DESCRIPTOR_CAP = BYTESINKiB(512u); // 524288 descriptor that are always gpu visible

// TODO : make a copy sampler

i8 initialize_gpu_descritor_heap(directx_device* device);

i8 set_gpu_descriptor_heaps(directx_cc* commandlist);

i8 signal_descriptor_size(u32 size);

i8 allocate_static_descriptors(D3D12_CPU_DESCRIPTOR_HANDLE* handle, u32 size);

D3D12_GPU_DESCRIPTOR_HANDLE copy_descriptor_to_heap(directx_device* device, D3D12_CPU_DESCRIPTOR_HANDLE* src,u32 count);

void shutdown_gpu_heap(void);


