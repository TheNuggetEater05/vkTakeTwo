#pragma once
#include <vulkan/vulkan.h>

namespace je
{
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily = UINT32_MAX;
		uint32_t presentFamily = UINT32_MAX;

		bool isComplete() const { return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX; }
	};

	class JEDevice
	{
	public:
		JEDevice(VkInstance instance, VkSurfaceKHR surface);
		~JEDevice();

		JEDevice(const JEDevice&) = delete;
		JEDevice& operator= (const JEDevice&) = delete;

		VkDevice device() { return _device; };

		QueueFamilyIndices findQueueFamilies();

	private:
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;

		VkQueue _graphicsQueue = VK_NULL_HANDLE;
		VkQueue _presentQueue = VK_NULL_HANDLE;

		VkInstance _instance = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;

		// Picks which lovely GPU we'll be using
		void choosePhysicalDevice();
		void createLogicalDevice();

		const char* getPhysicalDeviceType(uint32_t type) const;
	};
}