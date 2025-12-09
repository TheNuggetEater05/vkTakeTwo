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

	private:
		JEDevice& _jeDevice;
		JEWindow& _jeWindow;

		VkSwapchainKHR _swapchain;
		SwapchainSupportDetails _swapchainSupportDetails;

		void createSwapchain();
	};
}