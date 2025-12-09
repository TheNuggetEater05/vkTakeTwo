#pragma once
#include <vulkan/vulkan.h>

#include "je_types.hpp"

struct QueueFamilyIndices
{
	uint32_t graphicsFamily = UINT32_MAX;
	uint32_t presentFamily = UINT32_MAX;

	bool isComplete() const { return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX; }
};

struct SwapchainPackage
{
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;
};