#pragma once
#include <vulkan/vulkan.h>

namespace je
{
	class JEDevice
	{
	public:
		JEDevice(VkInstance instance);
		~JEDevice();

		JEDevice(const JEDevice&) = delete;
		JEDevice& operator= (const JEDevice&) = delete;

	private:
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;

		VkInstance _instance = VK_NULL_HANDLE;

		void choosePhysicalDevice();

		const char* getPhysicalDeviceType(uint32_t type) const;
	};
}