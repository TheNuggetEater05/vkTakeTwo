#pragma once

#include "je_swapchain.hpp"
#include "../core/je_device.hpp"

namespace je
{
	class JERenderPass
	{
	public:
		JERenderPass(JESwapchain& swapchain, JEDevice& device);
		~JERenderPass();

		JERenderPass(const JERenderPass&) = delete;
		JERenderPass& operator= (const JERenderPass&) = delete;

		VkRenderPass renderPass() const { return _renderPass; };

	private:
		JESwapchain& _jeSwapchain;
		JEDevice& _jeDevice;

		VkRenderPass _renderPass;

		void createRenderPass();
	};
}