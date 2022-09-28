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

#include "pipeline_state.hpp"
#include "shader.hpp"

#include "directx_types.INL"
#include "directx_backend.hpp"

#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "DXGI.lib")

// TODO : finish the back buffer clearing
// TEST
#include "buffer.hpp"
static directx_shader_module* module;
static directx_pipeline*	  pipeline;
static directx_buffer		  buffer;
static directx_buffer         index;
static u64					  buffersize;
static hk_vertex			  triangleVertices[] = { { { 0.0f, 0.25f, 0.5f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
													 { { 0.25f, -0.25f, 0.5f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
													 { { -0.25f, -0.25f, 0.5f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };
static u32 index_buffer[] = { 0, 1, 2 };
D3D12_INDEX_BUFFER_VIEW       index_view;
D3D12_VERTEX_BUFFER_VIEW	  vertex_view;


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
 * create all 3 type of directx_commandlist for further processing
 * \see queue_type.
 *
 * \param device directx_device to use for creation
 * \return H_OK on sucess and H_FAIL on failure
 */
i8 create_commandlist(directx_device* device);
/**
 * context destroy routine that is part of the shutdown routine.
 */
void context_destroy(void);
/**
 * command list destroy routine that is part of the backend shutdown.
 */
void commandlist_destroy(void);
/**
 * helper function to print the gpu specs to the console .
 *
 * \param adapter pointer to queried physical_device in directx_context
 */
void print_adapter_spec(IDXGIAdapter1* adapter); // print gpu specs
/**
 * directx_commandlist failure handle routine.
 *
 * \param fail_code corresponding commandlist_fails enum
 */
i8 commandlist_fail_handler(
	commandlist_fails fail_code); // command list fail handler since labels and gotos push me to c99 and I don't wike it

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

// LEGACY
/**
 * commandlist record function, this resets the commandlist with a new allocator and makes a it state to recording.
 *
 * \param commandlist* corresponding direct_commandlist pointer
 * \param type	type of queue used , used to get the approiate allocators
 */
i8 commandlist_record(directx_commandlist* commandlist, queue_type type);
/**
 * commandlist routine to end command allocation and close it for execution.
 *
 * \param	commandlist pointer to the concerned directx_commandlist
 * \param	type commandlist type concerned
 */
i8 commandlist_end_recording(directx_commandlist* commandlist, queue_type type);
/** singleton instance of directx context */
static directx_context* context;

i8 directx_initialize(renderer_backend* backend_ptr)
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
	ret_code = initiate_upload_structure();
	if (H_FAIL == ret_code)
	{
		HLEMER("upload structure creation failure");
		return ret_code;
	}

// TEST : buffer test
	buffersize = sizeof(triangleVertices);
	create_buffer(context, &buffer, triangleVertices, buffersize, VERTEX_RESOURCE);
	create_buffer(context, &index, index_buffer, 3 * sizeof(u32), INDEX_RESOURCE);

	vertex_view.BufferLocation = buffer.resource->GetGPUVirtualAddress();
	vertex_view.SizeInBytes	   = buffersize;
	vertex_view.StrideInBytes  = sizeof(hk_vertex);

	index_view.BufferLocation = index.resource->GetGPUVirtualAddress();
	index_view.SizeInBytes = 3;
	index_view.Format = DXGI_FORMAT_R32_UINT;

	return ret_code;
}

void directx_shutdown(renderer_backend* backend_ptr)
{
	HLINFO("Directx Shutdown");
	DEBUG_LAYER_SHUTDOWN();
	full_gpu_flush(&context->queue, HK_COMMAND_RENDER);

	context_destroy();
	command_context_shutdown(&context->queue);
	commandlist_destroy();
	command_buffer_pool_shutdown();
	swapchain_shutdown(&context->swapchain);
	hmemory_free(context, MEM_TAG_RENDERER);
	shutdown_upload_structure(context->queue.fence_val);

	// TEST
	destroy_pipeline_state(pipeline);
	release_buffer(&buffer);
	release_buffer(&index);
}

i8 directx_begin_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code = H_OK;

	queue_type type = HK_COMMAND_RENDER;
	ret_code		= prepare_commandlist_record(&context->commandlist[type]);
	if (H_OK != ret_code)
	{
		// TODO :stability work for this recovery
		HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
		return H_FAIL;
	}
	context->is_ready[type] = true;

	ret_code = set_render_target(&context->swapchain, &context->commandlist[HK_COMMAND_RENDER]);
	if (H_OK != ret_code)
	{
		HLWARN("swapchain preparation failure");
		return H_FAIL;
	}
	// TEST
	bind_pipeline_state(&context->commandlist[HK_COMMAND_RENDER], pipeline);
	bind_rendertarget_and_depth_stencil(&context->commandlist[HK_COMMAND_RENDER], &context->swapchain);

	clear_back_buffer(&context->commandlist[HK_COMMAND_RENDER], &context->swapchain, 0.1f, 0.1f, 0.1f, 1.0f);
	clear_depth_stencil(&context->commandlist[HK_COMMAND_RENDER], &context->swapchain);
	bind_scissor_rect(&context->commandlist[HK_COMMAND_RENDER], &context->swapchain);
	bind_view_port(&context->commandlist[HK_COMMAND_RENDER], &context->swapchain);

	context->commandlist[HK_COMMAND_RENDER].commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return ret_code;
}

i8 directx_end_frame(renderer_backend* backend_ptr, f64 delta_time)
{
	i8 ret_code = H_OK;

	context->commandlist[HK_COMMAND_RENDER].commandlist->IASetIndexBuffer(&index_view);
	context->commandlist[HK_COMMAND_RENDER].commandlist->IASetVertexBuffers(0, 1, &vertex_view);
	context->commandlist[HK_COMMAND_RENDER].commandlist->DrawInstanced(3, 1, 0, 0);
	

	if (true == context->is_ready[HK_COMMAND_COPY])
	{
		ret_code = end_commandlist_record(&context->commandlist[HK_COMMAND_COPY]);
		execute_command(context, &context->commandlist[HK_COMMAND_COPY]);
		next_frame_synchronization(&context->queue, &context->commandlist[HK_COMMAND_COPY]);
		context->is_ready[HK_COMMAND_COPY] = false;
	}

	if (true == context->is_ready[HK_COMMAND_COMPUTE])
	{
		ret_code = end_commandlist_record(&context->commandlist[HK_COMMAND_COMPUTE]);
		execute_command(context, &context->commandlist[HK_COMMAND_COPY]);
		next_frame_synchronization(&context->queue, &context->commandlist[HK_COMMAND_COMPUTE]);
		context->is_ready[HK_COMMAND_COMPUTE] = false;
	}
	set_present_target(&context->swapchain, &context->commandlist[HK_COMMAND_RENDER]);

	end_commandlist_record(&context->commandlist[HK_COMMAND_RENDER]);
	execute_command(context, &context->commandlist[HK_COMMAND_RENDER]);
	context->is_ready[HK_COMMAND_RENDER] = false;
	next_frame_synchronization(&context->queue, &context->commandlist[HK_COMMAND_RENDER]);


	ret_code = present_frame(&context->swapchain);

	return ret_code;
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
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		context->is_ready[i] = false;
	}

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

	ret_code = command_buffer_pool(&context->device, 2 * command_allocator_size);
	if (H_FAIL == ret_code)
	{
		HLEMER("haku command buffer pool creation failure");
		ret_code = context_fail_handler(context, command_allocator_fail);
		return ret_code;
	}
	HLINFO("haku command buffer pool created");

	ret_code = create_commandlist(&context->device);
	if (H_FAIL == ret_code)
	{
		HLEMER("commandlist creation failure");
		ret_code = context_fail_handler(context, commandlist_fail);
		return ret_code;
	}
	HLINFO("commandlist created");

	// TEST
	create_shader_module(&module);
	create_shader_byte_code(context, L"D:\\Haku\\bin\\BuiltIn_Vertex.cso", HK_VERTEX_SHADER, module);
	create_shader_byte_code(context, L"D:\\Haku\\bin\\BuiltIn_Pixel.cso", HK_PIXEL_SHADER, module);
	pipeline = create_pipeline_state(&context->device, module);

	return ret_code;
}

// helper to create commandlists in context
i8 create_commandlist(directx_device* device)
{
	i8		ret_code	 = H_OK;
	HRESULT api_ret_code = S_OK;

	directx_commandlist* commandlist = context->commandlist;
	directx_allocator*	 allocator	 = request_command_buffer(HK_COMMAND_RENDER);

	// render command list create
	api_ret_code = device->logical_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		allocator->allocator,
		nullptr,
		__uuidof(ID3D12GraphicsCommandList),
		(void**)&commandlist[HK_COMMAND_RENDER].commandlist);
	FRIENDLY_NAME(commandlist[HK_COMMAND_RENDER].commandlist, L"render commandlist");
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist creation failed");
		ret_code = commandlist_fail_handler(render_commandlist_fail);
		return ret_code;
	}
	api_ret_code = commandlist[HK_COMMAND_RENDER].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist close failed");
		ret_code = commandlist_fail_handler(copy_commandlist_fail);
		return ret_code;
	}
	commandlist[HK_COMMAND_RENDER].type				= HK_COMMAND_RENDER;
	commandlist[HK_COMMAND_RENDER].seeded_allocator = nullptr;
	commandlist[HK_COMMAND_RENDER].state			= COMMANDLIST_STALE;
	return_directx_allocator(allocator);

	// copy list create
	allocator	 = request_command_buffer(HK_COMMAND_COPY);
	api_ret_code = device->logical_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_COPY,
		allocator->allocator,
		nullptr,
		__uuidof(ID3D12GraphicsCommandList),
		(void**)&commandlist[HK_COMMAND_COPY].commandlist);
	if (S_OK != api_ret_code)
	{
		HLEMER("copy commandlist creation failed");
		ret_code = commandlist_fail_handler(copy_commandlist_fail);
		return ret_code;
	}
	api_ret_code = commandlist[HK_COMMAND_COPY].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist close failed");
		ret_code = commandlist_fail_handler(compute_commandlist_fail);
		return ret_code;
	}
	FRIENDLY_NAME(commandlist[HK_COMMAND_RENDER].commandlist, L"copy commandlist");
	commandlist[HK_COMMAND_COPY].type			  = HK_COMMAND_COPY;
	commandlist[HK_COMMAND_COPY].seeded_allocator = nullptr;
	commandlist[HK_COMMAND_COPY].state			  = COMMANDLIST_STALE;
	return_directx_allocator(allocator);

	// compute list create
	allocator	 = request_command_buffer(HK_COMMAND_COMPUTE);
	api_ret_code = device->logical_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_COMPUTE,
		allocator->allocator,
		nullptr,
		__uuidof(ID3D12GraphicsCommandList),
		(void**)&commandlist[HK_COMMAND_COMPUTE].commandlist);
	if (S_OK != api_ret_code)
	{
		HLEMER("compute commandlist creation failed");
		ret_code = commandlist_fail_handler(compute_commandlist_fail);
		return ret_code;
	}
	api_ret_code = commandlist[HK_COMMAND_COMPUTE].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("render commandlist close failed");
		ret_code = commandlist_fail_handler(clean_commandlist_fail);
		return ret_code;
	}
	FRIENDLY_NAME(commandlist[HK_COMMAND_RENDER].commandlist, L"compute commandlist");
	commandlist[HK_COMMAND_COMPUTE].type			 = HK_COMMAND_COMPUTE;
	commandlist[HK_COMMAND_COMPUTE].seeded_allocator = nullptr;
	commandlist[HK_COMMAND_COMPUTE].state			 = COMMANDLIST_STALE;
	return_directx_allocator(allocator);

	return ret_code;
}

// helper to clean commandlists
void commandlist_destroy(void)
{
	HLINFO("command list shutdown");
	directx_commandlist* commandlist = context->commandlist;
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		commandlist[i].commandlist->Release();
	}
}

i8 commandlist_record(directx_commandlist* commandlist, queue_type type)
{
	i8		ret_code	 = H_OK;
	HRESULT api_ret_code = S_OK;

	directx_allocator* alloc_ptr = request_command_buffer(type);

	api_ret_code = commandlist[type].commandlist->Reset(alloc_ptr->allocator, nullptr);
	if (S_OK != api_ret_code)
	{
		HLEMER("command list is not resetted");
		return H_FAIL;
	}
	commandlist[type].state = COMMANDLIST_RECORDING;

	return ret_code;
}

i8 commandlist_end_recording(directx_commandlist* commandlist, queue_type type)
{
	i8		ret_code		= H_OK;
	HRESULT api_ret_code	= S_OK;
	commandlist[type].state = COMMANDLIST_RECORDING_ENDED;
	api_ret_code			= commandlist[type].commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLEMER("command list closing fail");
		return H_FAIL;
	}

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

directx_commandlist* request_commandlist(queue_type type)
{
	directx_commandlist* list = nullptr;

	if (false == context->is_ready[type])
	{
		i8 ret_code = prepare_commandlist_record(&context->commandlist[type]);
		if (H_OK != ret_code)
		{
			// TODO :stability work for this recovery
			HLWARN("commandlist preparation failure, SLEEP CPU WORK HERE");
			return list;
		}
		context->is_ready[type] = true;
	}

	list = &context->commandlist[type];

	return list;
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

i8 commandlist_fail_handler(commandlist_fails fail_code)
{
	directx_commandlist* commandlist = context->commandlist;
	switch (fail_code)
	{
	case clean_commandlist_fail:
		commandlist[HK_COMMAND_COMPUTE].commandlist->Release();
	case compute_commandlist_fail:
		commandlist[HK_COMMAND_COPY].commandlist->Release();
	case copy_commandlist_fail:
		commandlist[HK_COMMAND_RENDER].commandlist->Release();
	case render_commandlist_fail:
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
