#include "command.hpp"
#include "graphics.hpp"
#include "defines.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include <limits>

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

