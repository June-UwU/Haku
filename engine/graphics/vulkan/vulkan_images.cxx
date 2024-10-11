#include "vulkan_images.hpp"
#include "defines.hpp"
#include "vulkan_device.hpp"

vulkan_images::vulkan_images(std::string name, std::shared_ptr<vulkan_device> device, VkImageCreateInfo& image_info)
	: name(name)
	, device(device)
	, memory(memory)
	, image(VK_NULL_HANDLE)
	, view(VK_NULL_HANDLE) {
	create_allocation(device, image_info);
	ASSERT(VK_NULL_HANDLE == image, "image can't be null handle");
	view = VK_NULL_HANDLE;
}

vulkan_images::vulkan_images(std::string name, std::shared_ptr<vulkan_device> device, VkImageCreateInfo& image_info, VkImageViewCreateInfo& view_info)
	: name(name)
	, memory(memory)
	, device(device)
	, image(VK_NULL_HANDLE)
	, view(VK_NULL_HANDLE) {
	create_allocation(device, image_info);
	ASSERT(VK_NULL_HANDLE != image, "image can't be null handle");
	view_info.image = image;
	VkResult result = vkCreateImageView(device->get_logical_device(), &view_info, nullptr, &view);
	VK_ASSERT(result, "failed to create image view..!");
}

vulkan_images::~vulkan_images() {
	device->free(image, memory);
	if (VK_NULL_HANDLE == view) {
		return;
	}

	vkDestroyImageView(device->get_logical_device(), view, nullptr);
}

vulkan_images::vulkan_images(vulkan_images&& rhs) {
	name   = rhs.name;
	image  = rhs.image;
	view   = rhs.view;
	memory = rhs.memory;
	device = rhs.device;
}

vulkan_images& vulkan_images::operator=(vulkan_images& rhs) {
	name   = rhs.name;
	image  = rhs.image;
	view   = rhs.view;
	memory = rhs.memory;
	device = rhs.device;

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

void vulkan_images::create_allocation(std::shared_ptr<vulkan_device> device, VkImageCreateInfo& image_info) {
	VkResult result = device->create_image(&image, &memory, image_info);
	VK_ASSERT(result, "failed to create image");
}
