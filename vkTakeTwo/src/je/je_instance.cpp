#include <SDL3/SDL_log.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>
#include <vector>

#include "je_instance.hpp"

namespace je
{
	JEInstance::JEInstance()
	{
		createInstance();
	}

	JEInstance::~JEInstance()
	{
		vkDestroyInstance(_instance, nullptr);
	}

	void JEInstance::createInstance()
	{
		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = nullptr;
		applicationInfo.pApplicationName = "Jayden Engine";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "No Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_4;

		uint32_t instanceExtensionCount = 0;
		const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);

		std::vector<const char*> instanceExtensions(instanceExtensionCount);
		for (uint32_t i = 0; i < instanceExtensionCount; i++)
		{
			instanceExtensions[i] = sdlExtensions[i];
		}

		SDL_Log("[+] SDL required extensions:");
		for (auto& extension : instanceExtensions)
		{
			SDL_Log("\t[>] %s", extension);
		}

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan instance");
	}
}
