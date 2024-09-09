#include "vulkan_renderpass.hpp"

vulkan_renderpass::vulkan_renderpass(std::string name, vulkan_device* device,std::vector<VkAttachmentDescription> attachments, 
        std::vector<VkSubpassDescription> subpass,std::vector<VkSubpassDependency> dependency) 
    : name(name) {
    gpu = device->get_logical_device();

    VkRenderPassCreateInfo create_info{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.subpassCount = subpass.size();
    create_info.pSubpasses = subpass.data();
    create_info.dependencyCount = dependency.size();
    create_info.pDependencies = dependency.data();

    VkResult result = vkCreateRenderPass(gpu,&create_info,nullptr,&renderpass);
    VK_ASSERT(result, "failed to create render pass"); 
}

vulkan_renderpass::~vulkan_renderpass() {
    vkDestroyRenderPass(gpu,renderpass,nullptr);
}

vulkan_renderpass::vulkan_renderpass(const vulkan_renderpass& rhs) {
    gpu = rhs.gpu;
    renderpass = rhs.renderpass;
    name = rhs.name;
}

vulkan_renderpass& vulkan_renderpass::operator=(const vulkan_renderpass& rhs) {
    gpu = rhs.gpu;
    renderpass = rhs.renderpass;
    name = rhs.name;

    return *this;
}

vulkan_renderpass::vulkan_renderpass(vulkan_renderpass&& rhs) {
    gpu = rhs.gpu;
    renderpass = rhs.renderpass;
    name = rhs.name;
}

vulkan_renderpass& vulkan_renderpass::operator=(vulkan_renderpass&& rhs) {
    gpu = rhs.gpu;
    renderpass = rhs.renderpass;
    name = rhs.name;

    return *this;
}

VkRenderPass vulkan_renderpass::get_renderpass() {
    return renderpass;
}