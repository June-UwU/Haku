#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vulkan_device.hpp"
#include <vector>
#include <string>

class vulkan_renderpass {
public:
    vulkan_renderpass(vulkan_device* device,std::vector<VkAttachmentDescription> attachments, 
        std::vector<VkSubpassDescription> subpass,std::vector<VkSubpassDependency> dependency);
    ~vulkan_renderpass();

    vulkan_renderpass(const vulkan_renderpass& rhs);
    vulkan_renderpass& operator=(const vulkan_renderpass& rhs);

    vulkan_renderpass(vulkan_renderpass&& rhs);
    vulkan_renderpass& operator=(vulkan_renderpass&& rhs);

    VkRenderPass get_renderpass();
private:
    std::string name;
    VkDevice gpu;
    VkRenderPass renderpass;
};