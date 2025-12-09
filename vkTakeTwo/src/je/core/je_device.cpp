#include "je_device.hpp"
#include <SDL3/SDL_log.h>

#include <vector>
#include <stdexcept>

namespace je
{
	JEDevice::JEDevice(VkInstance instance, VkSurfaceKHR surface) : _instance(instance), _surface(surface)
	{
		choosePhysicalDevice();
		createLogicalDevice();
	}

	JEDevice::~JEDevice()
	{
		vkDestroyDevice(_device, nullptr);
	}

	QueueFamilyIndices JEDevice::findQueueFamilies()
	{
		QueueFamilyIndices indices{};

		uint32_t queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyPropertyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

		for (uint32_t i = 0; i < queueFamilyPropertyCount; i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &presentSupport);
			if (presentSupport)
				indices.presentFamily = i;

			if (indices.isComplete())
				break;
		}

		return indices;
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

			if (physicalDeviceProperties.deviceType == 2)
			{
				_physicalDevice = physicalDevices[i];
				SDL_Log("\t^^ Hell yeah, discrete?!? We using this one :)");
			}

			if (_physicalDevice == VK_NULL_HANDLE)
			{
				_physicalDevice = physicalDevices[i];
			}
		}

		SDL_Log("[+] Well I really hope we found a discrete GPU, cus if not ur using integrated graphics");
	}

	void JEDevice::createLogicalDevice()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies();

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

		float queuePriority = 1.0f;

		if (queueFamilyIndices.graphicsFamily == queueFamilyIndices.presentFamily)
		{
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

			deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
		}
		else
		{
			VkDeviceQueueCreateInfo deviceGraphicsQueueCreateInfo{};
			deviceGraphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceGraphicsQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
			deviceGraphicsQueueCreateInfo.queueCount = 1;
			deviceGraphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

			deviceQueueCreateInfos.push_back(deviceGraphicsQueueCreateInfo);

			VkDeviceQueueCreateInfo devicePresentQueueCreateInfo{};
			devicePresentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			devicePresentQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.presentFamily;
			devicePresentQueueCreateInfo.queueCount = 1;
			devicePresentQueueCreateInfo.pQueuePriorities = &queuePriority;

			deviceQueueCreateInfos.push_back(devicePresentQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures physicalDeviceFeatures{};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

		deviceCreateInfo.enabledLayerCount = 0;

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");

		vkGetDeviceQueue(_device, queueFamilyIndices.graphicsFamily, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, queueFamilyIndices.presentFamily, 0, &_presentQueue);

		SDL_Log("[+] Created logical device!");
		SDL_Log("\tGraphics queue family: %u", queueFamilyIndices.graphicsFamily);
		SDL_Log("\tPresent queue family: %u", queueFamilyIndices.presentFamily);
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