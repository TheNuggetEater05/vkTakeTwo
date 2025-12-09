#pragma once
#include <vulkan/vulkan.h>

#include <vector>

#include "../core/je_types.hpp"
#include "../core/je_device.hpp"
#include "../core/je_window.hpp"

namespace je
{
	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> surfacePresentModes;
	};

	class JESwapchain
	{
	public:
		JESwapchain(JEDevice& device, JEWindow& window);
		~JESwapchain();

		JESwapchain(const JESwapchain&) = delete;
		JESwapchain& operator= (const JESwapchain&) = delete;

		VkSwapchainKHR swapchain() { return _swapchain; };
		SwapchainSupportDetails swapchainSupportDetails() const { return _swapchainSupportDetails; };
		std::vector<VkImage> swapchainImages() const { return _swapchainImages; };
		std::vector<VkImageView> swapchainImageViews() const { return _swapchainImageViews; };
		VkFormat swapchainImageFormat() const { return _swapchainImageFormat; };
		VkExtent2D swapchainExtent() const { return _swapchainExtent; };

	private:
		JEDevice& _jeDevice;
		JEWindow& _jeWindow;

		VkSwapchainKHR _swapchain;
		SwapchainSupportDetails _swapchainSupportDetails;

		std::vector<VkImage> _swapchainImages;
		std::vector<VkImageView> _swapchainImageViews;
		VkFormat _swapchainImageFormat{};
		VkExtent2D _swapchainExtent{};

		void createSwapchain();
		void createImageViews();
	};
}