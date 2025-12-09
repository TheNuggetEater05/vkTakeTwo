#pragma once

#include <memory>

#include "je_swapchain.hpp"

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

		JEDevice& _jeDevice;
		JEWindow& _jeWindow;
	};
}