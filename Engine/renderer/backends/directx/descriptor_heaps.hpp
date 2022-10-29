/*****************************************************************//**
 * \file   descriptor_heaps.hpp
 * \brief  haku descriptor heap abstractions
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "defines.hpp"
#include "directx_types.INL"

typedef struct descriptor_handle
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_ptr;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_ptr;
	u64 size;

	directx_descriptor_heap* heap_owner;
}descriptor_handle;

bool is_shader_visible(descriptor_handle* handle);

bool is_null(descriptor_handle* handle);


i8 create_descriptor_heap(directx_descriptor_heap* out_heap, u64 number_of_descriptors, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, bool is_shader_visible);

bool is_allocation_possible(directx_descriptor_heap* heap,u64 size);

descriptor_handle allocate(directx_descriptor_heap* heap, u64 size);

void free(descriptor_handle* handle);

void destroy_descriptor_heap(directx_descriptor_heap* heap);
