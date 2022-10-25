/*****************************************************************//**
 * \file   buffer.cpp
 * \brief  
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "buffer.hpp"
#include "generics/queue.hpp"
#include "core/logger.hpp"
#include "directx_backend.hpp"
#include "memory/hmemory.hpp"

typedef struct up_res
{
    u64 fence_val;

    ID3D12Resource* resource;
}up_res;


static queue_t upres_queue;

i8 initialize_buffer_structures(void)
{
    i8 ret_val = create_queue(&upres_queue, sizeof(up_res));
    
    if (H_OK != ret_val)
    {
        HLEMER("upload resource queue creation queue failure");
        return H_FAIL;
    }

    return H_OK;
}

void shutdown_buffer_structure()
{
    destroy_queue(&upres_queue);
}

i8 create_buffer(directx_device* device, directx_buffer* out_buffer, u64 width, u64 height, u64 depth, D3D12_RESOURCE_DIMENSION dimension)
{
    D3D12_HEAP_PROPERTIES heap_prop{};
    heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_prop.CreationNodeMask = 0;
    heap_prop.VisibleNodeMask = 0;
    

    out_buffer->desc.Dimension = dimension;
    out_buffer->desc.Format = DXGI_FORMAT_UNKNOWN;
    out_buffer->desc.Alignment = 0;
    out_buffer->desc.Width = width;
    out_buffer->desc.Height = height;
    out_buffer->desc.DepthOrArraySize = depth;
    out_buffer->desc.Format = DXGI_FORMAT_UNKNOWN;
    out_buffer->desc.SampleDesc.Count = 1;
    out_buffer->desc.SampleDesc.Quality = 0u;
    out_buffer->desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    out_buffer->desc.MipLevels = 1;
    out_buffer->desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


    HRESULT win32_rv = device->logical_device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &out_buffer->desc, 
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&out_buffer->resource));
    if (S_OK != win32_rv)
    {
        HLCRIT("failed to create resource");
        return H_FAIL;
    }
    out_buffer->state = D3D12_RESOURCE_STATE_COMMON;
    return H_OK;
}

void release_buffer(directx_buffer* buffer)
{
    buffer->resource->Release();
}

void transition_buffer(directx_buffer* buffer, u64 sub_resource, D3D12_RESOURCE_STATES after_state)
{
    directx_cc* cc = request_commandlist(HK_COMMAND_RENDER);

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = buffer->resource;
    barrier.Transition.StateBefore = buffer->state;
    barrier.Transition.StateAfter = after_state;
    barrier.Transition.Subresource = sub_resource;

    cc->commandlist->ResourceBarrier(1u, &barrier);

    buffer->state = after_state;
}

void copy_buffer(directx_buffer* dest, directx_buffer* src)
{
    directx_cc* cc = request_commandlist(HK_COMMAND_COPY);

    cc->commandlist->CopyResource(dest->resource, src->resource);
}

void resize_buffer(directx_buffer* buffer)
{
    D3D12_HEAP_PROPERTIES heap_prop{};
    heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_prop.CreationNodeMask = 0;
    heap_prop.VisibleNodeMask = 0;

    HRESULT win32_rv = get_device()->logical_device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &buffer->desc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buffer->resource));
    if (S_OK != win32_rv)
    {
        HLCRIT("failed to resize resource");
    }
}

void load_buffer(directx_buffer* buffer, void* data,u64 size, u64 sub_resource)
{
    up_res res;
    res.fence_val = get_current_fence_val();

    D3D12_HEAP_PROPERTIES heap_prop{};
    heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
    heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_prop.CreationNodeMask = 0;
    heap_prop.VisibleNodeMask = 0;

    directx_device* device = get_device();

    HRESULT win32_rv = device->logical_device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &buffer->desc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(&res.resource));
    if (S_OK != win32_rv)
    {
        HLEMER("failed to create upload resource");
        return;
    }

    u8* mapped_ptr = nullptr;
    D3D12_RANGE range{0};
// TODO : does sub resource th
    win32_rv = res.resource->Map(sub_resource, &range, (void**)&mapped_ptr);
    hmemory_copy(mapped_ptr, data, size);
    res.resource->Unmap(sub_resource, nullptr);

    enqueue(&upres_queue, &res);

    directx_cc* cc = request_commandlist(HK_COMMAND_COPY);
    cc->commandlist->CopyResource(buffer->resource, res.resource);
}

void reclaim_buffer(u64 fence_val)
{
    for_each_queue_t(&upres_queue)
    {
        up_res* res = (up_res*)entry->data;
        if (res->fence_val < fence_val)
        {
// FIX ME : this is broken
            //res->resource->Release();
        }
        else
        {
            return;
        }
    }
}






