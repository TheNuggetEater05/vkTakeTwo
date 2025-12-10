#pragma once

#include <vulkan/vulkan.h>

#include "../core/je_device.hpp"
#include <SDL3/SDL_log.h>

namespace je
{
	class JECommandPool
	{
	public:
		JECommandPool(JEDevice& jeDevice);
		~JECommandPool();

		JECommandPool(const JECommandPool&) = delete;
		JECommandPool& operator= (const JECommandPool&) = delete;

		VkCommandPool& commandPool() { return _commandPool; };

	private:
		JEDevice& _jeDevice;

		VkCommandPool _commandPool = VK_NULL_HANDLE;

		void createCommandPool();
	};
}