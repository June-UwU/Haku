#include "vulkan_images.hpp"
#include "defines.hpp"

vulkan_images::vulkan_images(std::string name, VkDevice device, VmaAllocation memory, VkImage image)
	: name(name)
	, image(image)
	, memory(memory) {
	ASSERT(VK_NULL_HANDLE == image, "image can't be null handle");
	view = VK_NULL_HANDLE;
}

vulkan_images::vulkan_images(std::string name, VkDevice device, VmaAllocation memory, VkImage image, VkImageViewCreateInfo& view_info)
	: name(name)
	, image(image)
	, memory(memory) {
	ASSERT(VK_NULL_HANDLE != image, "image can't be null handle");
	VkResult result = vkCreateImageView(device, &view_info, nullptr, &view);
	VK_ASSERT(result, "failed to create image view..!");
}

vulkan_images::vulkan_images(vulkan_images&& rhs) {
	name   = rhs.name;
	image  = rhs.image;
	view   = rhs.view;
	memory = rhs.memory;
}

vulkan_images& vulkan_images::operator=(vulkan_images& rhs) {
	name   = rhs.name;
	image  = rhs.image;
	view   = rhs.view;
	memory = rhs.memory;

	return *this;
}

VkImage vulkan_images::get_image() {
	return image;
}

VkImageView vulkan_images::get_view() {
	if (VK_NULL_HANDLE == view) {
		WARN << "name : " << name
			 << " looking up view for image where exist no view..!!\n";
	}

	return view;
}
