#pragma once
#include "vulkan_results.hpp"
#include "vulkan_renderpass.hpp"
#include "vulkan_device.hpp"

class vulkan_shader {
public:
	vulkan_shader(std::shared_ptr<vulkan_device>& device, std::string path, VkShaderStageFlagBits stage);
	~vulkan_shader();

	VkPipelineShaderStageCreateInfo get_shader_info();

private:
	std::vector<byte>			   byte_code;
	std::shared_ptr<vulkan_device> device;
	VkShaderStageFlagBits		   stage;
	VkShaderModule				   shader;
};

class vulkan_pipeline {
public:
	vulkan_pipeline(
		std::shared_ptr<vulkan_device>&				device,
		std::shared_ptr<vulkan_renderpass>&			renderpass,
		u32											width,
		u32											height,
		std::vector<std::shared_ptr<vulkan_shader>> shaders);
	~vulkan_pipeline();

	void bind(VkCommandBuffer cmd_buffer);
	void push_constants(VkCommandBuffer cmd, VkShaderStageFlags stage, u32 offset, u32 size, void* data);

private:
	std::shared_ptr<vulkan_device>	   device;
	VkPipeline						   pipeline;
	VkPipelineLayout				   layout;
	std::shared_ptr<vulkan_renderpass> renderpass;
};