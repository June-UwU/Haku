/*****************************************************************//**
 * \file   buffer.cpp
 * \brief  buffer inplementations 
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "buffer.hpp"
#include "core\logger.hpp"
#include "directx_backend.hpp"
#include "generics/hash_set.hpp"
#include "memory/hmemory.hpp"

/** internal structure to safely deallocate the upload resource */
typedef struct upload_tracker
{
	/** fence value till the resource was released, this will be used for proper release */
	u64 last_fence_val;

	/** table that hold the resource to keep it alive */
	hash_set_t* resource_table;
}upload_tracker;

static upload_tracker* tracker;

i8 initiate_upload_structure(void)
{
	tracker = (upload_tracker*)hmemory_alloc(sizeof(upload_tracker), MEM_TAG_RENDERER);
	tracker->resource_table = (hash_set_t*)hmemory_alloc(sizeof(upload_tracker), MEM_TAG_RENDERER);
	tracker->last_fence_val = 0;

	HAKU_CREATE_INT_HASH_SET(tracker->resource_table, ID3D12Resource*);

	return H_OK;
}

void shutdown_upload_structure(u64 fence_val)
{
	destroy_hash_table(tracker->resource_table);
	hmemory_free(tracker, MEM_TAG_RENDERER);
}

// TODO make the buffer elements usable
i8 create_buffer(const directx_context* context, directx_buffer* buffer, i8* data, u64 size, resource_type type)
{
	i8 ret_val = H_OK;
	HRESULT api_ret_val = S_OK;
	const directx_device* device = &context->device;

	ID3D12Resource* upload_resource = nullptr;

	D3D12_HEAP_PROPERTIES up_heap_prop;
	D3D12_HEAP_FLAGS up_heap_flag = D3D12_HEAP_FLAG_NONE;
	D3D12_RESOURCE_DESC resource_desc{};
	D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	
	D3D12_HEAP_PROPERTIES buff_heap_prop;
	D3D12_HEAP_FLAGS buff_heap_flag = D3D12_HEAP_FLAG_NONE;
	D3D12_RESOURCE_DESC buff_resource_desc{};
	D3D12_RESOURCE_STATES buff_init_state = D3D12_RESOURCE_STATE_GENERIC_READ;

	up_heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	up_heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	up_heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	up_heap_prop.CreationNodeMask = 0;
	up_heap_prop.VisibleNodeMask = 0;

	buff_heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	buff_heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
	buff_heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
	buff_heap_prop.CreationNodeMask = 0;
	buff_heap_prop.VisibleNodeMask = 0;
	
	
	switch (type)
	{
	/** defines a vertex resource */
	case VERTEX_RESOURCE:
	{
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 0;
		resource_desc.SampleDesc.Quality = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		buff_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buff_resource_desc.Alignment = 0;
		buff_resource_desc.Width = size;
		buff_resource_desc.Height = 1;
		buff_resource_desc.DepthOrArraySize = 1;
		buff_resource_desc.MipLevels = 1;
		buff_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		buff_resource_desc.SampleDesc.Count = 0;
		buff_resource_desc.SampleDesc.Quality = 1;
		buff_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buff_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		break;
	}
	/** defines a index resource */
	case INDEX_RESOURCE:
	{
		HLWARN("%s descriptions is not set up",RESOURCE_TYPE_STRING[type]);
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 0;
		resource_desc.SampleDesc.Quality = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		buff_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buff_resource_desc.Alignment = 0;
		buff_resource_desc.Width = size;
		buff_resource_desc.Height = 1;
		buff_resource_desc.DepthOrArraySize = 1;
		buff_resource_desc.MipLevels = 1;
		buff_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		buff_resource_desc.SampleDesc.Count = 0;
		buff_resource_desc.SampleDesc.Quality = 1;
		buff_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buff_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		break;
	}
	/** shader resource */
	case SHADER_RESOURCE:
	{
		HLWARN("%s descriptions is not set up", RESOURCE_TYPE_STRING[type]);
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 0;
		resource_desc.SampleDesc.Quality = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		buff_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buff_resource_desc.Alignment = 0;
		buff_resource_desc.Width = size;
		buff_resource_desc.Height = 1;
		buff_resource_desc.DepthOrArraySize = 1;
		buff_resource_desc.MipLevels = 1;
		buff_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		buff_resource_desc.SampleDesc.Count = 0;
		buff_resource_desc.SampleDesc.Quality = 1;
		buff_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buff_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		break;
	}
	/** constant resource */
	case CONST_RESOURCE:
	{
		HLWARN("%s descriptions is not set up", RESOURCE_TYPE_STRING[type]);
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 0;
		resource_desc.SampleDesc.Quality = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		buff_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buff_resource_desc.Alignment = 0;
		buff_resource_desc.Width = size;
		buff_resource_desc.Height = 1;
		buff_resource_desc.DepthOrArraySize = 1;
		buff_resource_desc.MipLevels = 1;
		buff_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		buff_resource_desc.SampleDesc.Count = 0;
		buff_resource_desc.SampleDesc.Quality = 1;
		buff_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buff_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		break;
	}
    /** unordered resource */
	case UNORDERED_RESOURCE:
	{
		HLWARN("%s descriptions is not set up", RESOURCE_TYPE_STRING[type]);
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 0;
		resource_desc.SampleDesc.Quality = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		buff_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buff_resource_desc.Alignment = 0;
		buff_resource_desc.Width = size;
		buff_resource_desc.Height = 1;
		buff_resource_desc.DepthOrArraySize = 1;
		buff_resource_desc.MipLevels = 1;
		buff_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		buff_resource_desc.SampleDesc.Count = 0;
		buff_resource_desc.SampleDesc.Quality = 1;
		buff_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buff_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		break;
	}
	default:
	{
		HLCRIT("Unknown type %s", RESOURCE_TYPE_STRING[type]);
		return H_FAIL;
	}
	}

	api_ret_val = device->logical_device->CreateCommittedResource(&up_heap_prop, up_heap_flag,
		&resource_desc, init_state, nullptr, IID_PPV_ARGS(&upload_resource));
	if (S_OK != api_ret_val)
	{
		HLCRIT("failure in creating upload heap for committed resource %s", RESOURCE_TYPE_STRING[type]);
		return H_FAIL;
	}



	api_ret_val  = device->logical_device->CreateCommittedResource(&buff_heap_prop, buff_heap_flag,
		&buff_resource_desc, buff_init_state, nullptr, IID_PPV_ARGS(&buffer->resource));
	if (S_OK != api_ret_val)
	{
		HLCRIT("failed to create the resource %s", RESOURCE_TYPE_STRING[type]);
		upload_resource->Release();
		return H_FAIL;
	}

// TODO COPY and make the resource release safely
	directx_commandlist* list = nullptr;
	ret_val = request_commandlist(list, HK_COMMAND_COPY);
	if (H_OK != ret_val)
	{
		HLEMER("request commandlist failed");
// TODO : make this initiate recovery routines
		return H_FAIL;
	}
	list->commandlist->CopyResource(buffer->resource, upload_resource);
	push_back(tracker->resource_table, (void*)&context->queue.fence_val, &upload_resource);

	return ret_val;
}

void release_buffer(directx_buffer* buffer)
{
	buffer->resource->Release();
}

i64 remove_stale_upload_buffer(u64 fence_val)
{
	i64 freed_resource_count = 0;
	u64 per_frame_res = 0;
	
	for (u64 i = tracker->last_fence_val; i <= freed_resource_count; i++)
	{
		hash_set_entry_t* ret_entry = find(tracker->resource_table, &i);
		if (nullptr == ret_entry)
		{
			continue;
		}
		for_queue_t(ret_entry->data, entry->next != NULL_PTR)
		{
			ID3D12Resource* res_ptr = (ID3D12Resource*)entry->data;
			res_ptr->Release();
			freed_resource_count++;
			per_frame_res++;
		}
		HLINFO("Released Per Frame Upload buffers\n \t frame sync value  : %d\n \t resource count : %d", i, per_frame_res);
		per_frame_res = 0;
		remove_entry(tracker->resource_table, &i);
	}

	tracker->last_fence_val = fence_val;
	return freed_resource_count;
}
