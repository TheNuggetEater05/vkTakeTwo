#include "je_device.hpp"
#include <SDL3/SDL_log.h>

#include <vector>
#include <stdexcept>

namespace je
{
	JEDevice::JEDevice(VkInstance instance) : _instance(instance)
	{
		choosePhysicalDevice();
	}

	JEDevice::~JEDevice()
	{

	}
	void JEDevice::choosePhysicalDevice()
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data());

		SDL_Log("[+] Physical devices:");
		for (uint32_t i = 0; i < physicalDevices.size(); i++)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties{};
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
			SDL_Log("\t[%u]: %s | [%s]", i, physicalDeviceProperties.deviceName, getPhysicalDeviceType(physicalDeviceProperties.deviceType));
		}
	}
	const char* JEDevice::getPhysicalDeviceType(uint32_t type) const
	{
		switch (type)
		{
		case 0:
			return "OTHER";
		case 1:
			return "INTEGRATED";
		case 2:
			return "DISCRETE";
		case 3:
			return "VIRTUAL";
		case 4:
			return "CPU";
		default:
			return "UNKNOWN";
		}
	}
}