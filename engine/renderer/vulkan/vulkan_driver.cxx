#include "vulkan_driver.hpp"
#include "../../types.hpp"
#include "../../logger.hpp"
#include "../../window/window.hpp"
#include "vulkan_defines.hpp"
#include <ranges>
#include <vector>

vulkan_driver::vulkan_driver(std::shared_ptr<window> p_window)
    : renderer(p_window) {
}


vulkan_driver::~vulkan_driver() {

}  
