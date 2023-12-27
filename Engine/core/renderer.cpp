#include "logger.hpp"
#include <renderer.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>

struct QueueFamilyIndices {
    u32 graphicsFamily  = INVALID_VALUE;
    u32 computeFamily   = INVALID_VALUE;
    u32 transferFamily  = INVALID_VALUE;
};


constexpr const char* VULKAN_VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";
static VkInstance instance;
static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
static VkDevice device; 
static QueueFamilyIndices queueFamilyIndices;
static VkQueue graphicsQueue;
static VkQueue computeQueue;
static VkQueue transferQueue;

bool checkValidationLayers() {
    const char* validationLayer = VULKAN_VALIDATION_LAYER;

    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount,nullptr);

    std::vector<VkLayerProperties> availableLayer(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount,availableLayer.data());

    bool layerFound = false;
    for(const auto& layerProperties: availableLayer) {
        if(0 == strcmp(validationLayer,layerProperties.layerName)) {
            layerFound = true;
            break;
        }
    }

    return layerFound;
}


[[nodiscard]] Status createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Haku";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Haku Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> instanceLayers{};

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#if defined(ENGINE_DEBUG)
    bool validationLayerSupport = checkValidationLayers();
    if(true == validationLayerSupport) {
        LOG_TRACE("Validation layer found!");
        instanceLayers.push_back(VULKAN_VALIDATION_LAYER);
    }
    else {
        LOG_CRITICAL("Validation layer not found!");
    }

    createInfo.enabledLayerCount = static_cast<u32>(instanceLayers.size());
    createInfo.ppEnabledLayerNames = instanceLayers.data();
#endif

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    VULKAN_OK(result, "Failed to create vulkan instance")

    u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    LOG_TRACE("Available extensions:");

    for(auto& extension : extensions) {
        LOG_TRACE(extension.extensionName);
    }

    return OK;
}


[[nodiscard]] s32 findGraphicsFamilies(const VkPhysicalDevice& device, VkQueueFlagBits family) {
    u32 queueFamilyCount = 0;
    vkGetPhysicalQueueFamilyProperties(device,&queueFamilyCount,nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalQueueFamilyProperties(device,&queueFamilyCount,queueFamilies.data());


    s32 queueIndex = INVALID_VALUE;

    u32 iter = 0;
    for(const auto& queueFamily: queueFamilies) {
        if(queueFamily.queueFlags & family) {
            queueIndex = iter;
            break;
        }
    }

    return queueIndex;
}

bool isCapable(const VkPhysicalDevice& device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device,&deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device,&deviceFeatures);

    queueFamilyIndices.graphicsFamily = findGraphicsFamilies(device,VK_QUEUE_GRAPHICS_BIT);
    ASSERT_VALID(queueFamilyIndices.graphicsFamily)

    queueFamilyIndices.computeFamily = findGraphicsFamilies(device,VK_QUEUE_COMPUTE_BIT);
    queueFamilyIndices.transferFamily = findGraphicsFamilies(device,VK_QUEUE_TRANSFER_BIT);

    return deviceProperties.deviceType == 
    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    deviceFeatures.geometryShader && 
    INVALID_VALUE != queueFamilyIndices.graphicsFamily;
}

[[nodiscard]] Status setupPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);

    if(0 == deviceCount) {
        LOG_CRITICAL("No suppported device found!");
        return ERROR;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance,&deviceCount,devices.data());

    for(const auto& device: devices) {
        if(true == isCapable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if(VK_NULL_HANDLE == physicalDevice) {
        LOG_CRITICAL("No suppported device found!");
        return ERROR;
    }

    return OK;
}

[[nodiscard]] Status createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

    if(INVALID_VALUE != queueFamilyIndices.graphicsFamily) {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    if(INVALID_VALUE != queueFamilyIndices.computeFamily) {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndices.computeFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    if(INVALID_VALUE != queueFamilyIndices.transferFamily) {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndices.transferFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};


    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    VkResult status = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    VULKAN_OK(status);

    if(INVALID_VALUE != queueFamilyIndices.graphicsFamily) {
        vkGetDeviceQueue(device,queueFamilyIndices.graphicsFamily,0,&graphicsQueue);
    }

    if(INVALID_VALUE != queueFamilyIndices.computeFamily) {
        vkGetDeviceQueue(device,queueFamilyIndices.computeFamily,0,&computeFamily);
    }

    if(INVALID_VALUE != queueFamilyIndices.transferFamily) {
        vkGetDeviceQueue(device,queueFamilyIndices.transferFamily,0,&transferQueue);
    }    

    return OK;
}

void destroyLogicalDevice() {
    vkDestroyDevice(device, nullptr);
}

Status shutdownInstance() {
    vkDestroyInstance(instance, nullptr);
    return OK;
}

Status initializeRenderer() {
    Status status = createInstance();
    ASSERT_OK(status)

    status = setupPhysicalDevice();
    ASSERT_OK(status)
    
    status = createLogicalDevice();
    ASSERT_OK(status)

    return OK;
}

void shutdownRenderer() {
    destroyLogicalDevice();
    shutdownInstance();
}
