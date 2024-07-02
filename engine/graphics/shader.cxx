#include "shader.hpp"
#include "defines.hpp"
#include "file_systems/file.hpp"
#include "device.hpp"

VkShaderModule create_shader_module(std::vector<byte>& byte_code) {
	ASSERT(0 != byte_code.size(), "provided shader byte code is invalid!!!");

	VkShaderModuleCreateInfo create_info{};
	create_info.sType	 = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = byte_code.size();
	create_info.pCode	 = reinterpret_cast<const u32*>(byte_code.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(get_device(), &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		WARN << "failed to create shader module \n";
	}

	return shader_module;
}

void destroy_shader_module(VkShaderModule& module) {
	vkDestroyShaderModule(get_device(), module, nullptr);
}