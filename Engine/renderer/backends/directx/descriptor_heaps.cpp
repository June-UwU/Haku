/*****************************************************************//**
 * \file   descriptor_heaps.cpp
 * \brief  heap impl
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "descriptor_heaps.hpp"
#include "directx_backend.hpp"
#include "core/logger.hpp"

const wchar_t* DESCRIPTOR_NAME[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]
{
	L"const, shader, unordered heap",
	L"sampler heap",
	L"render target heap",
	L"depth stencil heap"
};


void heap_free(directx_descriptor_heap* heap, descriptor_handle* handle);

bool is_shader_visible(descriptor_handle* handle)
{
	return handle->heap_owner->is_shader_visible;
}

bool is_null(descriptor_handle* handle)
{
	if (handle->cpu_ptr.ptr == 0u)
	{
		return true;
	}
	return false;
}

i8 create_descriptor_heap(directx_descriptor_heap* out_heap, u64 number_of_descriptors, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, bool is_shader_visible)
{
	directx_device* dev = get_device();
	
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
	heap_desc.NodeMask = 0u;
	heap_desc.Type = heap_type;



	D3D12_DESCRIPTOR_HEAP_FLAGS flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (true == is_shader_visible)
	{
		flag = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	
		if (D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER == heap_type && number_of_descriptors > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE)
		{
			number_of_descriptors = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;
		}
		else if (number_of_descriptors > D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1)
		{
			number_of_descriptors = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1;
		}
	}
	heap_desc.NumDescriptors = number_of_descriptors;

	HRESULT win32_rv = dev->logical_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&out_heap->heap));
	if (S_OK != win32_rv)
	{
		HLEMER("%ls failure",DESCRIPTOR_NAME[heap_type]);
		return H_FAIL;
	}
	FRIENDLY_NAME(out_heap->heap, DESCRIPTOR_NAME[heap_type]);

	i8 hk_rv = create_queue(&out_heap->block_list, sizeof(descriptor_block));
	if (H_OK != hk_rv)
	{
		HLEMER("failed to create block list");
		out_heap->heap->Release();
		return H_FAIL;
	}


	out_heap->capacity = number_of_descriptors;
	out_heap->cpu_base = out_heap->heap->GetCPUDescriptorHandleForHeapStart();
	if (true == is_shader_visible)
	{
		out_heap->gpu_base = out_heap->heap->GetGPUDescriptorHandleForHeapStart();
	}
	else
	{
		out_heap->gpu_base.ptr = 0u;
	}
	out_heap->heap_inc = dev->logical_device->GetDescriptorHandleIncrementSize(heap_type);
	out_heap->is_shader_visible = is_shader_visible;
	
	descriptor_block block{};
	block.cpu_base = out_heap->cpu_base;
	block.gpu_base = out_heap->gpu_base;
	block.size = number_of_descriptors;

	enqueue(&out_heap->block_list, &block);

	return H_OK;
}

bool is_allocation_possible(directx_descriptor_heap* heap,u64 size)
{
	if (heap->largest_block <= size)
	{
		return true;
	}
	return false;
}

descriptor_handle allocate(directx_descriptor_heap* heap, u64 size)
{
	descriptor_handle r_handle{};
	for_each_queue_t(&heap->block_list)
	{
		descriptor_block* block = (descriptor_block*)entry->data;
		if (block->size >= size)
		{
			// NOTE : these might need some tweaking for better usage
			if (block->size == heap->largest_block)
			{
				heap->largest_block -= size;
			}
			r_handle.heap_owner = heap;
			block->size - size;
			r_handle.cpu_ptr = block->cpu_base;

			if (true == heap->is_shader_visible)
			{
				r_handle.gpu_ptr = block->gpu_base;
			}
			else
			{
				r_handle.gpu_ptr.ptr = 0u;
			}
			r_handle.size = size;
			if (block->size > 0u)
			{
				block->cpu_base.ptr += size * heap->heap_inc;
				if (true == heap->is_shader_visible)
				{
					block->gpu_base.ptr += size * heap->heap_inc;
				}
			}
			else
			{
				remove_at(&heap->block_list, entry);
			}
			return r_handle;
		}
	}
	return r_handle;
}

void free(descriptor_handle* handle)
{
	heap_free(handle->heap_owner, handle);
}

void destroy_descriptor_heap(directx_descriptor_heap* heap)
{
	heap->heap->Release();
}


void heap_free(directx_descriptor_heap* heap, descriptor_handle* handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE end = {};
	end.ptr = handle->cpu_ptr.ptr;
	for_each_queue_t(&heap->block_list)
	{
		descriptor_block* block = (descriptor_block*)entry->data;
		if (block->cpu_base.ptr == end.ptr)
		{
			block->cpu_base = handle->cpu_ptr;
			block->size += handle->size;
			if (block->size >= heap->largest_block)
			{
				heap->largest_block = block->size;
			}
			if (true == heap->is_shader_visible)
			{
				block->gpu_base = handle->gpu_ptr;
			}
			return;
		}
	}
}
