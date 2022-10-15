/*****************************************************************//**
 * \file   cpu_descriptors.hpp
 * \brief  haku directx backend descriptor heap handler and allocator
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include "directx_types.INL"

constexpr const u64 INITIAL_DESCRIPTOR_PAGE_SIZE = BYTESINMiB(4u);

constexpr const u64 FAILED_ALLOC = -1;


typedef struct cdes
{
	i64 size;
	
	SIZE_T page_id;
	
	D3D12_CPU_DESCRIPTOR_HANDLE  ptr;
}cdesc;

i8 cpu_descriptor_allocator_initialize(void);

cdes cpu_desc_allocate(u32 size);

void cpu_desc_free(cdes* handle);

void cpu_descriptor_allocator_shutdown(void);

bool is_alloc_fail(cdes* desc);


void test_descriptor(void);