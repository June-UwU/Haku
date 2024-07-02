#include "command.hpp"
#include "graphics.hpp"
#include "defines.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include <limits>

VkCommandPool	command_pool;
VkCommandBuffer command_buffer;
VkSemaphore		image_available_sema;
VkSemaphore		render_finish_sema;
VkFence			in_flight_fence;

void start_render_pass(VkCommandBuffer buffer, u32 index) {
	VkRenderPassBeginInfo info{};
	info.sType			   = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.renderPass		   = get_render_pass();
	info.framebuffer	   = get_frame_buffer(index);
	info.renderArea.offset = { 0, 0 };
	info.renderArea.extent = get_swapchain_extend();

	VkClearValue clearColor = { { { 0.5f, 0.7f, 0.3f, 1.0f } } };
	info.clearValueCount	= 1;
	info.pClearValues		= &clearColor;

	vkCmdBeginRenderPass(buffer, &info, VK_SUBPASS_CONTENTS_INLINE);

	bind_pipeline(buffer,get_render_pipeline());
	set_extend_and_scissor_to_max(command_buffer);
	draw(command_buffer,3,1,0,0);
}

void record_command_buffer(VkCommandBuffer buffer, u32 image_index) {
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags			= 0;	   // Optional
	begin_info.pInheritanceInfo = nullptr; // Optional

	ASSERT(VK_SUCCESS == vkBeginCommandBuffer(command_buffer, &begin_info), "failed start recording command buffer");

	start_render_pass(buffer, image_index);
}

void bind_pipeline(VkCommandBuffer buffer, VkPipeline pipeline) {
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void set_extend_and_scissor_to_max(VkCommandBuffer buffer) {
	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width	  = static_cast<float>(get_swapchain_extend().width);
	viewport.height	  = static_cast<float>(get_swapchain_extend().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(buffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = get_swapchain_extend();
	vkCmdSetScissor(buffer, 0, 1, &scissor);
}

void draw(VkCommandBuffer buffer, u32 vertex_count, u32 instance, u32 first_vertex, u32 first_instance) {
	vkCmdDraw(buffer, vertex_count, instance, first_vertex, first_instance);
}

void end_recording_command_buffer(VkCommandBuffer buffer) {
	vkCmdEndRenderPass(buffer);
	ASSERT(VK_SUCCESS == vkEndCommandBuffer(buffer), "failed to end render pass");
}

void destroy_command_module() {
	vkDestroySemaphore(get_device(), image_available_sema, nullptr);
	vkDestroySemaphore(get_device(), render_finish_sema, nullptr);
	vkDestroyFence(get_device(), in_flight_fence, nullptr);
	vkDestroyCommandPool(get_device(), command_pool, nullptr);
}

bool create_command_buffer() {
	VkCommandBufferAllocateInfo alloc_info{};
	alloc_info.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool		  = command_pool;
	alloc_info.level			  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1; // TODO maybe multi command buffer thing

	auto result = vkAllocateCommandBuffers(get_device(), &alloc_info, &command_buffer);
	ASSERT(VK_SUCCESS == result, "failed to allocate command buffers");

	return true;
}

bool create_sync_objects() {
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
	auto img_avail = vkCreateSemaphore(get_device(), &semaphore_info, nullptr, &image_available_sema);
	ASSERT(VK_SUCCESS == img_avail, "failed to create image available semaphore..!");

	auto render_done = vkCreateSemaphore(get_device(), &semaphore_info, nullptr, &render_finish_sema);
	ASSERT(VK_SUCCESS == render_done, "failed to create render finish semaphore..!");

	auto in_flight = vkCreateFence(get_device(), &fence_info, nullptr, &in_flight_fence);
	ASSERT(VK_SUCCESS == in_flight, "failed to create in flight fence..!");

	return true;
}

void wait_for_frame() {
	vkWaitForFences(get_device(), 1, &in_flight_fence, VK_TRUE, std::numeric_limits<u64>::max());
	vkResetFences(get_device(), 1, &in_flight_fence);
}

u32 accquire_swap_image() {
	u32 image_index;
	vkAcquireNextImageKHR(get_device(),get_swap_chain(),std::numeric_limits<u64>::max(),image_available_sema,VK_NULL_HANDLE,&image_index);

	vkResetCommandBuffer(command_buffer, /*VkCommandBufferResetFlagBits*/ 0);
	return image_index;
}

VkCommandBuffer get_command_buffer() {
	return command_buffer;
}

void submit_work(u32 frame) {
	end_recording_command_buffer(command_buffer);
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore wait_semaphores[] = {image_available_sema};

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    
	submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VkSemaphore signal_semaphores[] = {render_finish_sema};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    auto submit_status = vkQueueSubmit(get_graphics_queue(), 1, &submit_info, in_flight_fence);
	ASSERT(VK_SUCCESS == submit_status,"failed to submit work to graphics queue");
}

void present_work(u32 frame) {
	VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore signal_semaphores[] = {render_finish_sema};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swap_chains[] = {get_swap_chain()};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;

    present_info.pImageIndices = &frame;

    vkQueuePresentKHR(get_present_queue(), &present_info);
}

bool initialize_command_module() {
	gpu_queue queue_family = find_queue_families(get_physical_device());

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType			   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags			   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = queue_family.graphics_index;

	auto result = vkCreateCommandPool(get_device(), &pool_info, nullptr, &command_pool);
	ASSERT(VK_SUCCESS == result, "failed to initialize command pool");

	bool command_buffer_status = create_command_buffer();
	ASSERT(command_buffer_status, "failed to initialize command buffer");

	bool sync_obj = create_sync_objects();
	ASSERT(sync_obj, "failed to create sync objects");

	return true;
}
