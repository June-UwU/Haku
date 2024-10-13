#include "vulkan_context.hpp"
#include "vulkan_pipeline.hpp"
#include "platform/window.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <string.h>

#include "vulkan_primitives.hpp"
static vulkan_pipeline*				  test = nullptr;
static std::shared_ptr<vulkan_buffer> vertex_buffer;
static std::shared_ptr<vulkan_buffer> index_buffer;

const std::vector<vertex> vertices = { { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
									   { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
									   { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
									   { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } } };
const std::vector<u16>	  indices  = { 0, 1, 2, 2, 3, 0 };

void print_available_extensions() {
	u32		 available_layer_count = 0;
	VkResult result				   = vkEnumerateInstanceExtensionProperties(nullptr, &available_layer_count, nullptr);
	VK_ASSERT(result, "failed to enumerate extension count..!!");

	std::vector<VkExtensionProperties> properties(available_layer_count);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &available_layer_count, properties.data());
	VK_ASSERT(result, "failed to enumerate extensions..!!");

	TRACE << "Available Extensions : \n";

	for (auto& extensions : properties) {
		TRACE << extensions.extensionName << "\n";
	}
}

std::vector<const char*> request_layers(std::vector<const char*>& requested_layers) {
#if defined(NDEBUG)
	return {};
#else
	u32 layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> available_layer(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layer.data());

	std::vector<const char*> layers_available;

	for (auto& layer : available_layer) {
		for (auto request : requested_layers) {
			if (0 == strcmp(layer.layerName, request)) {
				layers_available.push_back(request);
			}
		}
	}

	return layers_available;
#endif
}

std::vector<const char*> request_extensions() {
	u32			 glfw_extension_count = 0;
	const char** glfw_extensions	  = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*> extensions;

	for (u32 i = 0; i < glfw_extension_count; i++) {
		extensions.push_back(glfw_extensions[i]);
	}

	extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

VkResult create_debug_util_messenger(
	VkInstance								  instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks*			  pAllocator,
	VkDebugUtilsMessengerEXT*				  pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		WARN << "Vulkan Debug Extensions are not available..!!\n";
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroy_debug_util_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
#if defined(NDEBUG)
#else
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
#endif
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		severity,
	VkDebugUtilsMessageTypeFlagsEXT				type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void*										user_data) {
	switch (type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << "general info : ";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << "violation of specifications : ";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << "potential non-optimal usage of vulkan : ";
	} break;
	}

	switch (severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << callback_data->pMessage << "\n";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
		TRACE << callback_data->pMessage << "\n";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
		WARN << callback_data->pMessage << "\n";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
		ERROR << callback_data->pMessage << "\n";
	} break;
	default:
		break;
	}

	return VK_FALSE;
}

void setup_debug_instance(VkInstance instance, VkDebugUtilsMessengerEXT* messenger) {
#if defined(_DEBUG)
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	create_info.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debug_callback;
	create_info.pUserData		= nullptr; // Optional

	VkResult messenger_status = create_debug_util_messenger(instance, &create_info, nullptr, messenger);
	VK_ASSERT(messenger_status, "failed to create debug util messanger");
#else
#endif
}

vulkan_context::vulkan_context(u32 width, u32 height)
	: frame(0)
	, width(width)
	, height(height) {
	VkApplicationInfo application_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	application_info.pNext				= nullptr;
	application_info.pApplicationName	= "Haku";
	application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	application_info.pEngineName		= "Haku Engine";
	application_info.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	application_info.apiVersion			= VK_MAKE_VERSION(1, 2, 0);

	std::vector<const char*> requested_layer{ "VK_LAYER_KHRONOS_validation" };

	auto enable_layer = request_layers(requested_layer);
	for (auto val : enable_layer) {
		TRACE << val << "\n";
	}

	auto enable_extension = request_extensions();

	VkInstanceCreateInfo instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instance_create_info.pNext = nullptr;
	instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	instance_create_info.pApplicationInfo		 = &application_info;
	instance_create_info.enabledLayerCount		 = enable_layer.size();
	instance_create_info.ppEnabledLayerNames	 = enable_layer.data();
	instance_create_info.enabledExtensionCount	 = enable_extension.size();
	instance_create_info.ppEnabledExtensionNames = enable_extension.data();

	//	print_available_extensions();

	VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
	VK_ASSERT(result, "failed to create an instance..!");
	setup_debug_instance(instance, &debug_messenger);

	VkResult surface_created = glfwCreateWindowSurface(instance, get_window(), nullptr, &surface);
	VK_ASSERT(surface_created, "failed to create window surface..!!");

	device	  = std::make_shared<vulkan_device>(instance, surface);
	swapchain = std::make_shared<vulkan_swapchain>(width, height, surface, device);
	create_sync_parameter();
	reserve_command_buffer();
	make_default_context_objects(device);
}

vulkan_context::~vulkan_context() {
	device->wait_till_idle();
	destroy_sync_parameter();
	swapchain = nullptr;
	device	  = nullptr;
	vertex_buffer = nullptr;
	index_buffer  = nullptr;
	delete test;
	vkDestroySurfaceKHR(instance, surface, nullptr);
	destroy_debug_util_messenger(instance, debug_messenger, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VkInstance vulkan_context::get_instance() {
	return instance;
}

u32 vulkan_context::draw_frame() {
	const u32 size	= in_flight.size();
	u32		  index = frame % size;

	in_flight[index]->wait();
	in_flight[index]->reset();

	// TODO : implement not rendering if the application is minimized..
	u32				image_index		 = swapchain->accquire_image_index(device->get_logical_device(), image_available[index]);
	VkCommandBuffer recording_buffer = accquire_command_buffer(index);
	swapchain->start_renderpass(recording_buffer, image_index);

	// TODO : accquire command buffer here and start recording..
	test->bind(recording_buffer);

	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width	  = static_cast<float>(width);
	viewport.height	  = static_cast<float>(height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(recording_buffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { width, height };
	vkCmdSetScissor(recording_buffer, 0, 1, &scissor);

	vertex_buffer->bind(recording_buffer, 0, 0);
	index_buffer->bind_as_index(recording_buffer);

	vkCmdDrawIndexed(recording_buffer, 6, 1, 0, 0, 0);

	// submitting
	swapchain->end_renderpass(recording_buffer);
	VkResult result = vkEndCommandBuffer(recording_buffer);
	VK_ASSERT(result, "failed to end command buffer..!!");
	submit_command_buffer(index);
	present_frame(index, image_index);

	return frame;
}

void vulkan_context::create_sync_parameter() {
	const u32 size = swapchain->get_image_count();
	in_flight.resize(size);
	image_available.resize(size);
	render_complete.resize(size);

	VkSemaphoreCreateInfo semaphore_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0; // reserved for future use

	VkDevice gpu = device->get_logical_device();
	for (u32 i = 0; i < size; i++) {
		in_flight[i] = new vulkan_fence(gpu, true);
		vkCreateSemaphore(gpu, &semaphore_info, nullptr, &image_available[i]);
		vkCreateSemaphore(gpu, &semaphore_info, nullptr, &render_complete[i]);
	}
}

void vulkan_context::destroy_sync_parameter() {
	const u32 size = in_flight.size();

	VkDevice gpu = device->get_logical_device();
	for (u32 i = 0; i < size; i++) {
		delete in_flight[i];
		in_flight[i] = nullptr;
		vkDestroySemaphore(gpu, image_available[i], nullptr);
		vkDestroySemaphore(gpu, render_complete[i], nullptr);
	}
}

void vulkan_context::submit_command_buffer(u32 index) {
	VkSubmitInfo		 create_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	VkSemaphore			 wait_on_semaphore[] = { image_available[index] };
	VkPipelineStageFlags wait_stages[]		 = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	create_info.waitSemaphoreCount = 1;
	create_info.pWaitDstStageMask  = wait_stages;
	create_info.pWaitSemaphores	   = wait_on_semaphore;

	create_info.commandBufferCount = 1;
	create_info.pCommandBuffers	   = &command_buffer[index];

	VkSemaphore signal_semaphores[]	 = { render_complete[index] };
	create_info.signalSemaphoreCount = 1;
	create_info.pSignalSemaphores	 = signal_semaphores;

	device->submit_commands(create_info, in_flight[index]->get());
}

void vulkan_context::present_frame(u32 index, u32 image_index) {
	VkSwapchainKHR screen[] = { swapchain->get_swapchain() };

	VkPresentInfoKHR create_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	create_info.pNext			   = nullptr;
	create_info.waitSemaphoreCount = 1;
	create_info.pWaitSemaphores	   = &render_complete[index];
	create_info.swapchainCount	   = 1;
	create_info.pSwapchains		   = screen;
	create_info.pImageIndices	   = &image_index;

	device->present(create_info);
}

void vulkan_context::reserve_command_buffer() {
	u32 size = swapchain->get_image_count();
	command_buffer.resize(size);

	for (u32 i = 0; i < size; i++) {
		command_buffer[i] = device->get_graphics_command_buffer(true);
	}
}

VkCommandBuffer vulkan_context::accquire_command_buffer(u32 index) {
	ASSERT(index < command_buffer.size(), "out of bounds look up..!!");
	VkCommandBufferBeginInfo create_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	VkResult				 result = vkResetCommandBuffer(command_buffer[index], 0);
	VK_ASSERT(result, "failed to reset command buffer..!!");

	result = vkBeginCommandBuffer(command_buffer[index], &create_info);
	VK_ASSERT(result, "failed to begin command buffer..!!");

	return command_buffer[index];
}

void vulkan_context::create_renderpass() {
	std::vector<VkAttachmentDescription> attachment;

	VkAttachmentDescription swap_attachment{};
	swap_attachment.flags		   = 0;
	swap_attachment.format		   = swapchain->get_swapchain_format();
	swap_attachment.samples		   = VK_SAMPLE_COUNT_1_BIT;
	swap_attachment.loadOp		   = VK_ATTACHMENT_LOAD_OP_CLEAR;
	swap_attachment.storeOp		   = VK_ATTACHMENT_STORE_OP_STORE;
	swap_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
	swap_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	swap_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	swap_attachment.finalLayout	   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	attachment.push_back(swap_attachment);
}

bool vulkan_context::make_default_context_objects(std::shared_ptr<vulkan_device>& device) {
	auto vertex_shader = std::make_shared<vulkan_shader>(device, __HAKU_SHADER_PATH__ "default_frag_shader", VK_SHADER_STAGE_FRAGMENT_BIT);
	auto pixel_shader  = std::make_shared<vulkan_shader>(device, __HAKU_SHADER_PATH__ "default_vert_shader", VK_SHADER_STAGE_VERTEX_BIT);

	// testing shit..
	auto renderpass = swapchain->get_3d_renderpass();
	test			= new vulkan_pipeline(device, renderpass, width, height, { vertex_shader, pixel_shader });
	vertex_buffer	= std::make_shared<vulkan_buffer>(
		  "name",
		  device,
		  vertices.size() * sizeof(vertex),
		  (byte*)vertices.data(),
		  (VkBufferUsageFlags)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
	index_buffer = std::make_shared<vulkan_buffer>(
		"test_buffer index",
		device,
		indices.size() * sizeof(u16),
		(byte*)indices.data(),
		(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT));

	return true;
}
