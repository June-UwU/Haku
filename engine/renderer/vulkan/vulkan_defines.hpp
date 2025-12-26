#pragma once
#include <vulkan/vulkan.hpp>
#include "../../logger.hpp"

#define VALIDATE_RESULT(result, reason)                                        \
  if (VK_SUCCESS != (result)) {                                                \
    FATAL << "Vulkan error: " << result << "\nreason : " << reason << "\n";    \
    std::abort();}
