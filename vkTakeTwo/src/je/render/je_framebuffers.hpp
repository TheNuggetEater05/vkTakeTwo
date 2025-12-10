#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "je_swapchain.hpp"
#include "je_renderpass.hpp"
#include "../core/je_device.hpp"

namespace je
{
	class JEFramebuffers
	{
	public:
		JEFramebuffers(JESwapchain& jeSwapchain, JERenderPass& jeRenderPass, JEDevice& jeDevice);
		~JEFramebuffers();

		JEFramebuffers(const JEFramebuffers&) = delete;
		JEFramebuffers& operator= (const JEFramebuffers&) = delete;

		std::vector<VkFramebuffer>& framebuffers() { return _framebuffers; };
	private:
		JESwapchain& _jeSwapchain;
		JERenderPass& _jeRenderPass;
		JEDevice& _jeDevice;

		std::vector<VkFramebuffer> _framebuffers;

		void createFramebuffers();
	};
}