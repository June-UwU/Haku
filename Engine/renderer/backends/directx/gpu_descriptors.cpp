/*****************************************************************//**
 * \file   gpu_descriptors.cpp
 * \brief  haku gpu visible handler defines
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "gpu_descriptors.hpp"
#include "core/logger.hpp"
/**
 * visible heap structure that handles the heaps.
 */
typedef struct  visible_heaps
{
	/** sampler heaps that is gpu visible */
	ID3D12DescriptorHeap* sampler_heap;

	/** resource descriptor heap that is gpu visible */
	ID3D12DescriptorHeap* resource_heap;

	/** heap currently has head and written to till it is reset to the DESCRIPTOR_HEAP_START once it is done till the end*/
	u32 heap_head;

	/** heap head  */
	u32 static_head;

}visible_heaps;


static visible_heaps intern_heaps;
static u32 DESCRIPTOR_HEAP_START = 1024u;
static u32 DESCRITPOR_HEAP_END   = RESOURCE_DESCRIPTOR_CAP - 1u;

i8 initialize_gpu_descritor_heap(directx_device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC resource_heap_desc;
	resource_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	resource_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	resource_heap_desc.NumDescriptors = RESOURCE_DESCRIPTOR_CAP;
	resource_heap_desc.NodeMask = 0;

	HRESULT api_ret_val = device->logical_device->CreateDescriptorHeap(&resource_heap_desc, 
		__uuidof(ID3D12DescriptorHeap), (void**)&intern_heaps.resource_heap);
	if (S_OK != api_ret_val)
	{
		HLEMER("Failed to create gpu visible heap for resources");
		return H_FAIL;
	}

	D3D12_DESCRIPTOR_HEAP_DESC samplers_heap_desc;
	samplers_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	samplers_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	samplers_heap_desc.NumDescriptors = SAMPLER_CAP;
	samplers_heap_desc.NodeMask = 0;

	api_ret_val = device->logical_device->CreateDescriptorHeap(&samplers_heap_desc,
		__uuidof(ID3D12DescriptorHeap), (void**)&intern_heaps.sampler_heap);
	if (S_OK != api_ret_val)
	{
		intern_heaps.resource_heap->Release();
		HLEMER("failed to create gpu visible sampler heap");
		return H_FAIL;
	}

	intern_heaps.heap_head = DESCRIPTOR_HEAP_START;
	intern_heaps.static_head = 0;

	return H_OK;
}

i8 set_gpu_descriptor_heaps(directx_commandlist* commandlist)
{
	ID3D12DescriptorHeap* descriptors[2u]{ intern_heaps.resource_heap,intern_heaps.sampler_heap };
	commandlist->commandlist->SetDescriptorHeaps(2u, descriptors);
	return H_OK;
}

i8 signal_descriptor_size(u32 size)
{
	if ((intern_heaps.heap_head + size) <= DESCRITPOR_HEAP_END)
	{
		return S_OK;
	}
	else
	{
		intern_heaps.heap_head = DESCRIPTOR_HEAP_START;
		return S_OK;
	}
}

i8 allocate_static_descriptors(D3D12_CPU_DESCRIPTOR_HANDLE* handle,u32 size)
{
	if ((intern_heaps.static_head + size) <= DESCRIPTOR_HEAP_START)
	{
		handle->ptr = intern_heaps.resource_heap->GetCPUDescriptorHandleForHeapStart().ptr + size;
		intern_heaps.static_head + size;
		return H_OK;
	}
	HLEMER("GPU visible heap static region overflow..! \n\t Allocated : %d, \n\t Capacity : %d,\n\t Request : %d", intern_heaps.static_head, DESCRIPTOR_HEAP_START, size);
	return H_FAIL;
}

D3D12_GPU_DESCRIPTOR_HANDLE copy_descriptor_to_heap(directx_device* device, D3D12_CPU_DESCRIPTOR_HANDLE* src, u32 count)
{
	if ((intern_heaps.heap_head + count) > DESCRITPOR_HEAP_END) 
	{
		intern_heaps.heap_head = DESCRIPTOR_HEAP_START;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dest{};
	dest.ptr = intern_heaps.resource_heap->GetCPUDescriptorHandleForHeapStart().ptr + intern_heaps.heap_head;

	device->logical_device->CopyDescriptorsSimple(count, dest, *src, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_GPU_DESCRIPTOR_HANDLE ret_handle;
	ret_handle.ptr = intern_heaps.resource_heap->GetGPUDescriptorHandleForHeapStart().ptr + intern_heaps.heap_head;
	intern_heaps.heap_head += count;

	return ret_handle;
}

void shutdown_gpu_heap(void)
{
	intern_heaps.resource_heap->Release();
	intern_heaps.sampler_heap->Release();
}
