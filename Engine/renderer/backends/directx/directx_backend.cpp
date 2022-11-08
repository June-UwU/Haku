/*****************************************************************/ /**
* \file   directx_backend.cpp
* \brief  directx 12 backend for directx
*
* \author June
* \date   September 2022
*********************************************************************/
#include "directx.hpp"
#include "swapchain.hpp"
#include "debuglayer.hpp"
#include "core/logger.hpp"
#include "command_context.hpp"
#include "memory/hmemory.hpp"
#include "command_buffer_pool.hpp"
#include "directx_types.INL"
#include "directx_backend.hpp"
#include "renderer/renderer_types.inl"
#include "engine_shaders.hpp"
#include "descriptor_heaps.hpp"

#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib")


#include "resources.hpp"
#include "helpers.hpp"
#include "rootsig.hpp"
#include "pso.hpp"

static u64					  buffersize;
static hk_vertex			  triangleVertices[] = { { { 0.0f, 0.25f, 0.5f,1.0f }, { 1.0f, 0.0f, 0.0f, 0.4f } },
													 { { 0.25f, -0.25f, 0.5f,1.0f }, { 0.0f, 1.0f, 0.0f, 0.4f } },
													 { { -0.25f, -0.25f, 0.5f ,1.0f}, { 0.0f, 0.0f, 1.0f, 0.4f } } };
static u32 index_buffer[] = { 0, 1, 2 };
static ID3D12PipelineState* pso = nullptr;
static ID3D12RootSignature* rootsig = nullptr;
ID3D12Resource* vertex_resource = nullptr;
ID3D12Resource* index_resource = nullptr;
ID3D12Resource* upvertex_resource = nullptr;
ID3D12Resource* upindex_resource = nullptr;


static ID3D12Resource* global_const[frame_count];
static u8* const_mapped_ptr[frame_count];
static global_transforms glob_transforms;


/** directx context creation failure codes */
typedef enum context_fails
{
	/** directx_commandlist creation failure */
	commandlist_fail,
	/** directx_command_allocator creation failure */
	command_allocator_fail,
	/** directx_swapchain failure */
	swapchain_fail,
	/** directx_queue creation failure */
	command_fail,
	/** device creation failure (logical device) */
	fail_device,
	/** device creation failure (physical device) */
	fail_adapter,
	/** dxgi factory creation failure */
	fail_factory_6
} context_fails;

/** directx_commandlist creation failure codes */
typedef enum commandlist_fails
{
	/** directx_commandlist HK_RENDER commandlist failure */
	render_commandlist_fail,

	/** copy commandlist HK_COPY commandlist failure */
	copy_commandlist_fail,

	/** compute commandlist HK_COMPUTE commandlist failure */
	compute_commandlist_fail,

	/** code to clean all commandlist on failure */
	clean_commandlist_fail
} commandlist_fails;

/**
 * helper function to create a directx_device for internal purpose.
 *
 * \return H_OK on sucess and H_FAIL on failure
 */
i8 create_device(void);

// internal functions that are used to sent up context and help with other things
/**
 * the initial routine part of the initialization code that is resposible for directx_context creation.
 */
i8 create_context(void);

/**
 * context destroy routine that is part of the shutdown routine.
 */
void context_destroy(void);

/**
 * helper function to print the gpu specs to the console .
 *
 * \param adapter pointer to queried physical_device in directx_context
 */
void print_adapter_spec(IDXGIAdapter1* adapter); // print gpu specs

/**
 * routine to clean up and handle potential directx_device failures.
 *
 * \param fail_code failure code for direct_device
 * \return haku return code
 */
i8 device_fail_handler(context_fails fail_code);

/**
 * directx_context creation failure handle routine.
 *
 * \param context pointer to the corresponding directx_context
 * \param fail_code failure code for specfic instance for context_fails
 */
i8 context_fail_handler(directx_context* context, context_fails fail_code); // context fail handler

// TEST : this is descriptor heap test var
static directx_descriptor_heap rv_heap;

/** singleton instance of directx context */
static directx_context* context;

i8 directx_initialize(renderer_backend* backend_ptr,void* data)
{
	i8 ret_code = H_OK;

	HLINFO("Directx initialize");
	ENABLE_DEBUG_LAYER();

	ret_code = create_context();
	if (H_FAIL == ret_code)
	{
		HLEMER("directx_context creation failure");
		return ret_code;
	}

	ret_code = create_descriptor_heap(&rv_heap, 4023, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	if (H_FAIL == ret_code)
	{
		HLEMER("shader visible heap creation failure");
		return ret_code;
	}
	
	ret_code = initialize_engine_shader();
	if (H_FAIL == ret_code)
	{
		HLEMER("failed to initialze shader");
		return H_FAIL;
	}
	
	glob_transforms.projection_matix = DirectX::XMMatrixIdentity();
	glob_transforms.view_matrix = DirectX::XMMatrixIdentity();
	for (u32 i = 0; i < frame_count; i++)
	{
		global_const[i] = create_resource(1u, sizeof(global_transforms), 1u, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_HEAP_TYPE_UPLOAD,
			D3D12_HEAP_FLAG_NONE, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_NONE);
		if (nullptr == global_const[i])
		{
			HLCRIT("failed creating global  const buffer");
			return H_FAIL;
		}


		D3D12_RANGE range{};
		HRESULT win32_rv = global_const[i]->Map(0u, &range, (void**)&const_mapped_ptr[i]);
		if(S_OK != win32_rv)
		{
			HLEMER("failed to map ptr");;
			return H_FAIL;
		}

		FRIENDLY_NAME(global_const[i], L"Global constant buffers");

		hmemory_copy(const_mapped_ptr[i], &glob_transforms, sizeof(global_transforms));
	}



// TEST 
	D3D12_ROOT_PARAMETER parameter[10u]{};
	D3D12_DESCRIPTOR_RANGE range[10]{};
	
	parameter[0] = make_rootdescriptors(0u, 0u, D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_ALL);

	rootsig = create_rootsignature(1u, parameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	D3D12_RASTERIZER_DESC raster = make_rasterize_desc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, false, D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, true, false, false, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	D3D12_BLEND_DESC blend = default_blend_desc();

	DXGI_FORMAT format[8u]{};
	format[0] = DXGI_FORMAT_R8G8B8A8_UNORM;


	D3D12_DEPTH_STENCIL_DESC ds_desc = make_depth_stencil(true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS, false,
		D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK, { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS }
	, { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS });

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = make_pso_desc(rootsig, get_shader(FULL_TRIANGLE_VERTEX_SHADER), get_shader(FULL_TRIANGLE_PIXEL_SHADER), nullptr,
		nullptr, nullptr, raster, blend, ds_desc, format, 0xFFFFFFFF, 1u, 1, 0u);

	pso = create_pipelinestate(pso_desc);

	
	vertex_resource = create_resource(1u,sizeof(hk_vertex) * 3u, 1, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_HEAP_FLAG_NONE, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_FLAG_NONE);


	index_resource = create_resource(1u, sizeof(u32) * 3u, 1, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_HEAP_FLAG_NONE, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_FLAG_NONE);


	upvertex_resource = create_resource(1u, sizeof(hk_vertex) * 3u, 1, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_HEAP_FLAG_NONE, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_NONE);


	upindex_resource = create_resource(1u, sizeof(u32) * 3u, 1, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_HEAP_FLAG_NONE, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_NONE);


	u8* vertex_map = nullptr;
	D3D12_RANGE uprange{};
	upvertex_resource->Map(0u, &uprange, (void**)&vertex_map);
	hmemory_copy(vertex_map, triangleVertices, sizeof(hk_vertex) * 3u);
	upvertex_resource->Unmap(0u, nullptr);

	u8* index_map = nullptr;
	upindex_resource->Map(0u, &uprange, (void**)&vertex_map);
	hmemory_copy(vertex_map, index_buffer, sizeof(u32) * 3u);
	upindex_resource->Unmap(0u, nullptr);

	context->curr_cc = request_dxcc();
	if (nullptr == context->curr_cc)
	{
		// TODO :stability work for this recovery
		HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
		return H_FAIL;
	}

	context->curr_cc->commandlist->CopyResource(vertex_resource, upvertex_resource);

	context->curr_cc->commandlist->CopyResource(index_resource, upindex_resource);


	end_commandlist_record(context->curr_cc);
	execute_command(context, context->curr_cc);
	next_frame_synchronization(&context->queue, context->curr_cc);


	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	
	full_gpu_flush(&context->queue, HK_COMMAND_RENDER);

// TEST : 
	pso->Release();
	rootsig->Release();

	shutdown_engine_shader();
	destroy_descriptor_heap(&rv_heap);
	context_destroy();
	command_context_shutdown(&context->queue);
	command_buffer_pool_shutdown();
	swapchain_shutdown(&context->swapchain);
	for (u8 i = 0; i < frame_count; i++)
	{
		global_const[i]->Unmap(0u, {});
		global_const[i]->Release();
	}
	hmemory_free(context, MEM_TAG_RENDERER);
}

i8 directx_begin_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code = H_OK;


	context->curr_cc = request_dxcc();
	if (nullptr == context->curr_cc)
	{
		// TODO :stability work for this recovery
		HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
		return H_FAIL;
	}

	ret_code = set_render_target(&context->swapchain, context->curr_cc);
	if (H_OK != ret_code)
	{
		HLWARN("swapchain preparation failure");
		return H_FAIL;
	}
	bind_rendertarget_and_depth_stencil(context->curr_cc, &context->swapchain);

	clear_back_buffer(context->curr_cc, &context->swapchain, 0.1f, 0.1f, 0.1f, 1.0f);
	clear_depth_stencil(context->curr_cc, &context->swapchain);
	bind_scissor_rect(context->curr_cc, &context->swapchain);
	bind_view_port(context->curr_cc, &context->swapchain);

	context->curr_cc->commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->curr_cc->commandlist->SetGraphicsRootSignature(rootsig);
	context->curr_cc->commandlist->SetPipelineState(pso);
	context->curr_cc->commandlist->SetDescriptorHeaps(1u, &rv_heap.heap);
	
	return ret_code;
}

i8 directx_end_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code = H_OK;


	D3D12_VERTEX_BUFFER_VIEW vert_view{};
	vert_view.BufferLocation = vertex_resource->GetGPUVirtualAddress();
	vert_view.SizeInBytes = sizeof(hk_vertex) * 3u;
	vert_view.StrideInBytes = sizeof(hk_vertex);

	D3D12_INDEX_BUFFER_VIEW ind_view{};
	ind_view.BufferLocation = index_resource->GetGPUVirtualAddress();
	ind_view.SizeInBytes = sizeof(u32) * 3u;
	ind_view.Format = DXGI_FORMAT_R32_UINT;


	context->curr_cc->commandlist->SetGraphicsRootConstantBufferView(0u, global_const[context->queue.fence_val % frame_count]->GetGPUVirtualAddress());
	context->curr_cc->commandlist->IASetIndexBuffer(&ind_view);
	context->curr_cc->commandlist->IASetVertexBuffers(0, 1, &vert_view);
	context->curr_cc->commandlist->DrawIndexedInstanced(3, 1, 0, 0, 0);


	set_present_target(&context->swapchain, context->curr_cc);

	end_commandlist_record(context->curr_cc);
	execute_command(context, context->curr_cc);
	next_frame_synchronization(&context->queue, context->curr_cc);
	
	
	for (u32 i = 0; i < frame_count; i++)
	{
		hmemory_copy(const_mapped_ptr[i], &glob_transforms, sizeof(global_transforms));
	}

	ret_code = present_frame(&context->swapchain);

	return ret_code;
}

i8 directx_update_global_transforms(renderer_backend* backend, void* data)
{
	for (u32 i = 0; i < frame_count; i++)
	{
		hmemory_copy(const_mapped_ptr[i], data, sizeof(global_transforms));
	}

	return H_OK;
}

i8 directx_resize(renderer_backend* backend_ptr, u16 height, u16 width)
{
	i8 ret_code = full_gpu_flush(&context->queue, HK_COMMAND_RENDER);
	if (H_OK != ret_code)
	{
		HLWARN("directx gpu flush failed");
		return H_FAIL;
	}

	ret_code = swapchain_resize(context, &context->swapchain, width, height);
	if (H_OK != ret_code)
	{
		HLWARN("swapchain resize failure");
		return H_FAIL;
	}
	return H_OK;
}

// INTERNAL HELPERS

// helper function to destroy device
void context_destroy(void)
{
	HLINFO("context shutdown");
	context->device.logical_device->Release();
	context->device.physical_device->Release();
	context->device.factory->Release();
}

// helper function to create device and swap chain
i8 create_context(void)
{
	HLINFO("device creation");
	i8 ret_code = H_OK;

	HRESULT api_ret_code = S_OK;

	context = (directx_context*)hmemory_alloc(sizeof(directx_context), MEM_TAG_RENDERER);

	ret_code = create_device();
	if (H_FAIL == ret_code)
	{
		HLEMER("directx device creation failure");
		ret_code = context_fail_handler(context, command_fail);
		return ret_code;
	}
	HLINFO("directx device created");

	ret_code = command_context_initialize(context);
	if (H_FAIL == ret_code)
	{
		HLEMER("directx command context creation failure");
		ret_code = context_fail_handler(context, command_fail);
		return ret_code;
	}
	HLINFO("directx command context created");

	ret_code = swapchain_initialize(context);
	if (H_FAIL == ret_code)
	{
		HLEMER("directx command context creation failure");
		ret_code = context_fail_handler(context, swapchain_fail);
		return ret_code;
	}
	HLINFO("directx command context created");

	ret_code = command_buffer_pool(&context->device, 4 * command_allocator_size);
	if (H_FAIL == ret_code)
	{
		HLEMER("haku command buffer pool creation failure");
		ret_code = context_fail_handler(context, command_allocator_fail);
		return ret_code;
	}
	HLINFO("haku command buffer pool created");

	
	return ret_code;
}


i8 create_device(void)
{
	i8 ret_code = H_OK;

	directx_device* device = &context->device;

	IDXGIFactory1* factory_1 = nullptr;

	HRESULT api_ret_code = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory_1);

	if (S_OK != api_ret_code)
	{
		HLEMER("Factory 1 dxgi creation failure");
		ret_code = H_FAIL;
		return ret_code;
	}

	api_ret_code = factory_1->QueryInterface(__uuidof(IDXGIFactory6), (void**)&device->factory);

	if (S_OK != api_ret_code)
	{
		HLEMER("dxgi factory 6 is not supported");
		factory_1->Release();
		ret_code = device_fail_handler(fail_factory_6);
		return ret_code;
	}

	api_ret_code = device->factory->EnumAdapterByGpuPreference(
		0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter1), (void**)&device->physical_device);

	if (S_OK != api_ret_code)
	{
		HLEMER("enumeration of adapter failure");
		factory_1->Release();
		ret_code = device_fail_handler(fail_adapter);
		return ret_code;
	}
	print_adapter_spec(device->physical_device);

	api_ret_code = D3D12CreateDevice(
		device->physical_device, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&device->logical_device);
	if (S_OK != api_ret_code)
	{
		HLEMER("device creation failure");
		factory_1->Release();
		ret_code = device_fail_handler(fail_device);
		return ret_code;
	}
	HLINFO("Device initailized");
	FRIENDLY_NAME(device->logical_device, L"DirectX device");

	factory_1->Release();
	return ret_code;
}

directx_cc* request_commandlist()
{
	directx_cc* list = nullptr;
	{
		context->curr_cc =  request_dxcc();
		
		if (nullptr == context->curr_cc)
		{
			// TODO :stability work for this recovery
			HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
			return nullptr;
		}
	}

	list = context->curr_cc;

	return list;
}

directx_device* get_device()
{
	return &context->device;
}

u64 get_current_fence_val(void)
{
	return context->queue.fence_val;
}

// FAIL HANDLERS

i8 device_fail_handler(context_fails fail_code)
{
	switch (fail_code)
	{
	case fail_device:
		context->device.factory->Release();
	case fail_adapter:
		context->device.physical_device->Release();
	case fail_factory_6:
		break;
	}
	return H_FAIL;
}

i8 context_fail_handler(directx_context* context, context_fails fail_code)
{
	switch (fail_code)
	{
	case commandlist_fail:
		command_buffer_pool_shutdown();
	case command_allocator_fail:
		swapchain_shutdown(&context->swapchain);
	case swapchain_fail:
		command_context_shutdown(&context->queue);
	case command_fail:
		context_destroy();
		break;
	}
	return H_FAIL;
}

// MISC

// helper function that prints adapter features
void print_adapter_spec(IDXGIAdapter1* adapter)
{
	DXGI_ADAPTER_DESC1 desc;
	adapter->GetDesc1(&desc);
	char gpu_char[128];
	HLINFO("Direct GPU detection");
	HLINFO("GPU		: %ls", desc.Description);
}
