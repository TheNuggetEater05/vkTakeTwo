#include <SDL3/SDL_log.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "je_instance.hpp"

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	SDL_Log("validation layer: %s", pCallbackData->pMessage);

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

namespace je
{
	JEInstance::JEInstance()
	{
		createInstance();
	}

	JEInstance::~JEInstance()
	{
		if (enableValidationLayers)
			DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);

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

		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		SDL_Log("[+] SDL required extensions:");
		for (auto& extension : instanceExtensions)
		{
			SDL_Log("\t[>] %s", extension);
		}

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::vector<const char*> validationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
		}

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (enableValidationLayers)
		{
			instanceCreateInfo.enabledLayerCount = validationLayers.size();
			instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan instance");
	}
	void je::JEInstance::setupValidationLayers()
	{
		
	}
	void JEInstance::setupDebugMessenger()
	{
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional

		if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug messenger!");
	}
}
