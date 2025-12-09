#include "je_swapchain.hpp"

#include <algorithm>
#include <stdexcept>

namespace je
{
	JESwapchain::JESwapchain(JEDevice& device, JEWindow& window) : _jeDevice(device), _jeWindow(window)
	{

		createSwapchain();
	}

	JESwapchain::~JESwapchain()
	{
		for (auto view : _swapchainImageViews)
			vkDestroyImageView(_jeDevice.device(), view, nullptr);

		vkDestroySwapchainKHR(_jeDevice.device(), _swapchain, nullptr);
	}

	void JESwapchain::createSwapchain()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_jeDevice.physicalDevice(), _jeWindow.surface(), &_swapchainSupportDetails.surfaceCapabilities);
		
		uint32_t surfaceFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_jeDevice.physicalDevice(), _jeWindow.surface(), &surfaceFormatCount, nullptr);

		_swapchainSupportDetails.surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_jeDevice.physicalDevice(), _jeWindow.surface(), &surfaceFormatCount, _swapchainSupportDetails.surfaceFormats.data());

		uint32_t surfacePresentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(_jeDevice.physicalDevice(), _jeWindow.surface(), &surfacePresentModeCount, nullptr);

		_swapchainSupportDetails.surfacePresentModes.resize(surfacePresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_jeDevice.physicalDevice(), _jeWindow.surface(), &surfacePresentModeCount, _swapchainSupportDetails.surfacePresentModes.data());

		// set imageFormat and imageColorSpace
		VkFormat selectedFormat = VK_FORMAT_MAX_ENUM;
		VkColorSpaceKHR selectedColorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;
		SDL_Log("[+] Swapchain stuff:");
		SDL_Log("\t[+] Surface formats:");
		for (uint32_t i = 0; i < _swapchainSupportDetails.surfaceFormats.size(); i++)
		{
			SDL_Log("\t\t[%u] Format: %u", i, _swapchainSupportDetails.surfaceFormats[i].format);

			if (_swapchainSupportDetails.surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
			{
				selectedFormat = VK_FORMAT_B8G8R8A8_SRGB;
				selectedColorSpace = _swapchainSupportDetails.surfaceFormats[i].colorSpace;
				SDL_Log("\t\t ^^ yay we found it, using ts rn");
			}

			if (selectedFormat == VK_FORMAT_MAX_ENUM)
			{
				selectedFormat = _swapchainSupportDetails.surfaceFormats[i].format;
				selectedColorSpace = _swapchainSupportDetails.surfaceFormats[i].colorSpace;
			}
		}

		// set presentMode
		VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
		SDL_Log("\t[+] Present modes:");
		for (uint32_t i = 0; i < _swapchainSupportDetails.surfacePresentModes.size(); i++)
		{
			SDL_Log("\t\t[%u] Present mode: %u", i, _swapchainSupportDetails.surfacePresentModes[i]);
			if (_swapchainSupportDetails.surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				selectedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				SDL_Log("\t\t^^ picked MAILBOX");
			}

			if (_swapchainSupportDetails.surfacePresentModes[i] == VK_PRESENT_MODE_FIFO_KHR && selectedPresentMode != VK_PRESENT_MODE_MAILBOX_KHR)
			{
				selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
				SDL_Log("\t\t^^ picked FIFO");
			}

			if (_swapchainSupportDetails.surfacePresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && selectedPresentMode != VK_PRESENT_MODE_MAILBOX_KHR && selectedPresentMode != VK_PRESENT_MODE_FIFO_KHR)
			{
				selectedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				SDL_Log("\t\t^^ picked IMMEDIATE");
			}
		}

		// set imageExtent
		VkExtent2D selectedImageExtent;
		if (_swapchainSupportDetails.surfaceCapabilities.currentExtent.width != UINT32_MAX)
		{
			selectedImageExtent = _swapchainSupportDetails.surfaceCapabilities.currentExtent;
		}
		else
		{
			VkExtent2D extent;
			extent.width = std::clamp(_jeWindow.width(), _swapchainSupportDetails.surfaceCapabilities.minImageExtent.width, _swapchainSupportDetails.surfaceCapabilities.maxImageExtent.width);
			extent.height = std::clamp(_jeWindow.height(), _swapchainSupportDetails.surfaceCapabilities.minImageExtent.height, _swapchainSupportDetails.surfaceCapabilities.maxImageExtent.height);

			selectedImageExtent = extent;
		}

		// set imageCount
		uint32_t selectedImageCount = _swapchainSupportDetails.surfaceCapabilities.minImageCount + 1;
		if (_swapchainSupportDetails.surfaceCapabilities.maxImageCount > 0 && selectedImageCount > _swapchainSupportDetails.surfaceCapabilities.maxImageCount)
			selectedImageCount = _swapchainSupportDetails.surfaceCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = _jeWindow.surface();
		swapchainCreateInfo.minImageCount = selectedImageCount;
		swapchainCreateInfo.imageFormat = selectedFormat;
		swapchainCreateInfo.imageColorSpace = selectedColorSpace;
		swapchainCreateInfo.imageExtent = selectedImageExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices queueFamilyIndices = _jeDevice.findQueueFamilies();
		uint32_t indices[] = { queueFamilyIndices.graphicsFamily, queueFamilyIndices.presentFamily };
		if (queueFamilyIndices.graphicsFamily == queueFamilyIndices.presentFamily)
		{
			
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}

		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = indices;

		swapchainCreateInfo.preTransform = _swapchainSupportDetails.surfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = selectedPresentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		SDL_Log("[+] Created Swapchain!");
		if (vkCreateSwapchainKHR(_jeDevice.device(), &swapchainCreateInfo, nullptr, &_swapchain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swapchain");

		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(_jeDevice.device(), _swapchain, &imageCount, nullptr);

		_swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_jeDevice.device(), _swapchain, &imageCount, _swapchainImages.data());

		_swapchainImageFormat = selectedFormat;
		_swapchainExtent = selectedImageExtent;
	}

	void JESwapchain::createImageViews()
	{
		_swapchainImageViews.resize(_swapchainImages.size());
		
		for (uint32_t i = 0; i < _swapchainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = _swapchainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = _swapchainImageFormat;
			imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY };
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(_jeDevice.device(), &imageViewCreateInfo, nullptr, &_swapchainImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create swapchain image views");
		}
	}
}