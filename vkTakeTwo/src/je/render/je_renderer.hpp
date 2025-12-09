#pragma once

#include <memory>

#include "je_swapchain.hpp"
#include "je_renderpass.hpp"

namespace je
{
	class JERenderer
	{
	public:
		JERenderer(JEDevice& device, JEWindow& window);
		~JERenderer();

		JERenderer(const JERenderer&) = delete;
		JERenderer& operator= (const JERenderer&) = delete;
	private:
		std::unique_ptr<JESwapchain> _jeSwapchain;
		std::unique_ptr<JERenderPass> _jeRenderPass;

		JEDevice& _jeDevice;
		JEWindow& _jeWindow;
	};
}