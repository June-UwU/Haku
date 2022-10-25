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

#include "shader.hpp"
#include "pipeline_state.hpp"


#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib")

#include "buffer.hpp"

// TODO : finish the back buffer clearing
// TEST
static directx_shader_module* module;
static directx_pipeline*	  pipeline;

static directx_buffer		    buffer;
static directx_buffer         index;
static directx_buffer         g_const;

static D3D12_VERTEX_BUFFER_VIEW vert_view;
static D3D12_INDEX_BUFFER_VIEW ind_view;

static u64					  buffersize;
static hk_vertex			  triangleVertices[] = { { { 0.0f, 0.25f, 0.5f,1.0f }, { 1.0f, 0.0f, 0.0f, 0.4f } },
													 { { 0.25f, -0.25f, 0.5f,1.0f }, { 0.0f, 1.0f, 0.0f, 0.4f } },
													 { { -0.25f, -0.25f, 0.5f ,1.0f}, { 0.0f, 0.0f, 1.0f, 0.4f } } };
static u32 index_buffer[] = { 0, 1, 2 };


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

/** singleton instance of directx context */
static directx_context* context;
static global_transforms transforms;


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

	ret_code = initialize_buffer_structures();

// TEST : buffer test
	buffersize = sizeof(triangleVertices);
	create_buffer(&context->device, &buffer, sizeof(hk_vertex) * 3u, 1u, 1u, D3D12_RESOURCE_DIMENSION_BUFFER);
	create_buffer(&context->device, &index, sizeof(u32) * 3u, 1u, 1u, D3D12_RESOURCE_DIMENSION_BUFFER);

	transition_buffer(&buffer, 0u, D3D12_RESOURCE_STATE_COPY_DEST);
	load_buffer(&buffer, triangleVertices, sizeof(hk_vertex) * 3u, 0u);
	transition_buffer(&buffer, 0u, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	
	transition_buffer(&index, 0u, D3D12_RESOURCE_STATE_COPY_DEST);
	load_buffer(&index, index_buffer, sizeof(u32) * 3u, 0u);
	transition_buffer(&index, 0u, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	vert_view.BufferLocation = buffer.resource->GetGPUVirtualAddress();
	vert_view.SizeInBytes = 3 * sizeof(hk_vertex);
	vert_view.StrideInBytes = sizeof(hk_vertex);

	ind_view.BufferLocation = buffer.resource->GetGPUVirtualAddress();
	ind_view.Format = DXGI_FORMAT_R32_UINT;
	ind_view.SizeInBytes = sizeof(u32) * 3u;


	end_commandlist_record(context->curr_cc[HK_COMMAND_RENDER]);
	execute_command(context, context->curr_cc[HK_COMMAND_RENDER]);
	next_frame_synchronization(&context->queue, context->curr_cc[HK_COMMAND_RENDER]);

	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	
	full_gpu_flush(&context->queue, HK_COMMAND_RENDER);

	context_destroy();
	command_context_shutdown(&context->queue);
	command_buffer_pool_shutdown();
	swapchain_shutdown(&context->swapchain);
	hmemory_free(context, MEM_TAG_RENDERER);
	shutdown_buffer_structure();
	// TEST
	destroy_pipeline_state(pipeline);
	release_buffer(&index);
	release_buffer(&buffer);
}

i8 directx_begin_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code = H_OK;


	context->curr_cc[HK_COMMAND_RENDER] = request_dxcc(HK_COMMAND_RENDER);
	if (nullptr == context->curr_cc[HK_COMMAND_RENDER])
	{
		// TODO :stability work for this recovery
		HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
		return H_FAIL;
	}

	ret_code = set_render_target(&context->swapchain, context->curr_cc[HK_COMMAND_RENDER]);
	if (H_OK != ret_code)
	{
		HLWARN("swapchain preparation failure");
		return H_FAIL;
	}
	// TEST
	bind_pipeline_state(context->curr_cc[HK_COMMAND_RENDER], pipeline);
	bind_rendertarget_and_depth_stencil(context->curr_cc[HK_COMMAND_RENDER], &context->swapchain);

	clear_back_buffer(context->curr_cc[HK_COMMAND_RENDER], &context->swapchain, 0.1f, 0.1f, 0.1f, 1.0f);
	clear_depth_stencil(context->curr_cc[HK_COMMAND_RENDER], &context->swapchain);
	bind_scissor_rect(context->curr_cc[HK_COMMAND_RENDER], &context->swapchain);
	bind_view_port(context->curr_cc[HK_COMMAND_RENDER], &context->swapchain);

	context->curr_cc[HK_COMMAND_RENDER]->commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return ret_code;
}

i8 directx_end_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code = H_OK;
	// TODO this is not needed here at the moment

	context->curr_cc[HK_COMMAND_RENDER]->commandlist->IASetVertexBuffers(0u, 1u, &vert_view);
	context->curr_cc[HK_COMMAND_RENDER]->commandlist->IASetIndexBuffer(&ind_view);


	context->curr_cc[HK_COMMAND_RENDER]->commandlist->DrawInstanced(3, 1, 0, 0);
	

	if (nullptr != context->curr_cc[HK_COMMAND_COPY])
	{
		ret_code = end_commandlist_record(context->curr_cc[HK_COMMAND_COPY]);
		execute_command(context, context->curr_cc[HK_COMMAND_COPY]);
		next_frame_synchronization(&context->queue, context->curr_cc[HK_COMMAND_COPY]);
	}

	if (nullptr != context->curr_cc[HK_COMMAND_COMPUTE])
	{
		ret_code = end_commandlist_record(context->curr_cc[HK_COMMAND_COMPUTE]);
		execute_command(context, context->curr_cc[HK_COMMAND_COPY]);
		next_frame_synchronization(&context->queue, context->curr_cc[HK_COMMAND_COMPUTE]);
	}
	set_present_target(&context->swapchain, context->curr_cc[HK_COMMAND_RENDER]);

	end_commandlist_record(context->curr_cc[HK_COMMAND_RENDER]);
	execute_command(context, context->curr_cc[HK_COMMAND_RENDER]);
	next_frame_synchronization(&context->queue, context->curr_cc[HK_COMMAND_RENDER]);


	ret_code = present_frame(&context->swapchain);

	for (u32 i = 0; i < HK_COMMAND_MAX; i++)
	{
		if (nullptr != context->curr_cc[i])
		{
			context->curr_cc[i] = nullptr;
		}
	}

	return ret_code;
}

// FIX ME : implement this
i8 directx_update_global_transforms(renderer_backend* backend_ptr, void* ptr)
{
	return H_FAIL;
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

	// TEST
	create_shader_module(&module);
	create_shader_byte_code(context, L"D:\\Haku\\bin\\BuiltIn_Vertex.cso", HK_VERTEX_SHADER, module);
	create_shader_byte_code(context, L"D:\\Haku\\bin\\BuiltIn_Pixel.cso", HK_PIXEL_SHADER, module);
	pipeline = create_pipeline_state(&context->device, module);

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

directx_cc* request_commandlist(queue_type type)
{
	directx_cc* list = nullptr;
	// TODO : this is not right
	if (nullptr == context->curr_cc[type])
	{
		context->curr_cc[type] =  request_dxcc(type);
		
		if (nullptr == context->curr_cc[type])
		{
			// TODO :stability work for this recovery
			HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
			return nullptr;
		}
	}

	list = context->curr_cc[type];

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
