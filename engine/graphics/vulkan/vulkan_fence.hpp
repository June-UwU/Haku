#pragma once
#include "types.hpp"
#include "defines.hpp"
#include "vulkan_results.hpp"

class vulkan_fence {
public:
	vulkan_fence(VkDevice device, bool make_signaled);
	~vulkan_fence();

	void reset();
	void wait();
	void wait_time_out(u64 ms);

	VkFence get();

private:
	VkDevice device;
	VkFence fence;
};