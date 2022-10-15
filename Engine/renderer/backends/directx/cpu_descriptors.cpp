/*****************************************************************//**
 * \file   cpu_descriptors.cpp
 * \brief  haku directx cpu descriptor heap handler and allocator
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "cpu_descriptors.hpp"
#include "generics/queue.hpp"
#include "directx_backend.hpp"
#include "core/logger.hpp"

typedef struct db
{
	u64 size;

	D3D12_CPU_DESCRIPTOR_HANDLE base;
}db;


typedef struct dpg
{
	SIZE_T id;
	
	u64 increment_size;
	
	u64 lb_size;

	queue_t free_list;

	ID3D12DescriptorHeap* heap;
}dpg;

static queue_t page_list;

i8 create_page(dpg* page, D3D12_DESCRIPTOR_HEAP_TYPE type);

void destroy_page(dpg* page);

i8 page_allocate(dpg* page,cdes* ptr, u64 size);

void page_free(dpg* page,cdes* ptr);


i8 cpu_descriptor_allocator_initialize(void)
{
	i8 ret_val = create_queue(&page_list, sizeof(dpg));
	if (H_FAIL == ret_val)
	{
		HLEMER("page list creation failure");
		return H_FAIL;
	}
	return H_OK;
}

cdes cpu_desc_allocate(u32 size)
{
	cdes  ret_val;
	ret_val.size = FAILED_ALLOC;
	for_each_queue_t(&page_list)
	{
		dpg* pg = (dpg*)entry->data;
		if (pg->lb_size >= size)
		{
			i8 h_ret = page_allocate(pg, &ret_val, size);
			if (H_FAIL == h_ret)
			{
				ret_val.size = FAILED_ALLOC;
			}
			else
			{
				return ret_val;
			}
		}
	}
	dpg page;
	i8 h_ret = create_page(&page, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (H_FAIL == h_ret)
	{
		HLEMER("page creation failure..!!");
		return ret_val;
	}
	h_ret = page_allocate(&page, &ret_val, size);
	if (H_FAIL == h_ret)
	{
		HLEMER("page allocation failure");
	}
	enqueue(&page_list, &page);
	return ret_val;
}

void cpu_desc_free(cdes* handle)
{
	for_each_queue_t(&page_list)
	{
		dpg* pg = (dpg*)entry->data;
		if (pg->id == handle->page_id)
		{
			page_free(pg, handle);
			return;
		}
	}
	HLCRIT("failed to find the page \n \t page_id : %d\n \t size : %d", handle->page_id, handle->size);
}

void cpu_descriptor_allocator_shutdown(void)
{
	for_each_queue_t(&page_list)
	{
		destroy_page((dpg*)entry->data);
	}
	destroy_queue(&page_list);
}

bool is_alloc_fail(cdes* desc)
{
	if (FAILED_ALLOC == desc->size)
	{
		return true;
	}
	return false;
}

void test_descriptor(void)
{
	cpu_descriptor_allocator_initialize();

	const u64 TEST_RANGE = 4u;

// TODO : validate the proper merging of blocks
	cdesc desc[TEST_RANGE];
	for (u64 i = 0; i < TEST_RANGE; i++)
	{
		desc[i] = cpu_desc_allocate(512u);
	}
	
	for (u64 i = 0; i < TEST_RANGE; i++)
	{
		cpu_desc_free(&desc[i]);
	}

	cpu_descriptor_allocator_shutdown();
}


i8 create_page(dpg* page, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	directx_device* dev = get_device();
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc;
	heap_desc.NodeMask = 0;
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heap_desc.NumDescriptors = INITIAL_DESCRIPTOR_PAGE_SIZE;
	heap_desc.Type = type;

	HRESULT api_ret_val = dev->logical_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&page->heap));
	if (S_OK != api_ret_val)
	{
		HLEMER("page creation failure");
		return H_FAIL;
	}
	i8 ret_val = create_queue(&page->free_list, sizeof(db));
	if (H_FAIL == ret_val)
	{
		HLEMER("queue creation failure");
		page->heap->Release();
		return H_FAIL;
	}
	page->increment_size = dev->logical_device->GetDescriptorHandleIncrementSize(type);
	page->lb_size = INITIAL_DESCRIPTOR_PAGE_SIZE;
	page->id = page->heap->GetCPUDescriptorHandleForHeapStart().ptr;
	HLINFO("Page created \n \t heap : %p", page->heap);
	return H_OK;
}

void destroy_page(dpg* page)
{
	HLINFO("Page release \n \t heap : %p", page->heap);
	page->heap->Release();
	destroy_queue(&page->free_list);
}

i8 page_allocate(dpg* page, cdes* ptr, u64 size)
{
	bool allocated = false;
	u64 lb_size = 0;
	for_each_queue_t(&page->free_list)
	{
		db* block = (db*)entry->data;
		if (block->size >= size && false == allocated)
		{
			ptr->page_id = page->id;
			ptr->size = size;
			ptr->ptr = block->base;
			allocated = true;
			if (0 == (block->size - size))
			{
				remove_at(&page->free_list, entry);
			}
			else
			{
				block->size -= size;
				block->base.ptr = block->base.ptr + (page->increment_size * size);
			}
			HLINFO("page allocate \n \t heap : %p\n \t size : %d", page->heap, size);
		}
		if (block->size > lb_size)
		{
			lb_size = block->size;
		}
	}
	page->lb_size = lb_size;
	return H_OK;
}

void page_free(dpg* page, cdes* ptr)
{
	D3D12_CPU_DESCRIPTOR_HANDLE lh;
	lh.ptr = ptr->ptr.ptr + (ptr->size * page->increment_size);
	u64 lb_size = 0;
	bool merged = false;
	
	for_each_queue_t(&page->free_list)
	{
		db* block = (db*)entry->data;
		if (lh.ptr == block->base.ptr)
		{
			block->base = ptr->ptr;
			block->size += ptr->size;
			merged = true;
		}
		if (lb_size < block->size)
		{
			lb_size = block->size;
		}
	}
	
	if (false == merged)
	{
		if (lb_size > ptr->size)
		{
			lb_size = ptr->size;
		}
		db block;
		block.base = ptr->ptr;
		block.size = ptr->size;
		enqueue(&page->free_list, &block);
	}
	page->lb_size = lb_size;
}
