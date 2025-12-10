#include "je_commandpool.hpp"

#include <stdexcept>

namespace je
{
	JECommandPool::JECommandPool(JEDevice& jeDevice) : _jeDevice(jeDevice)
	{
		createCommandPool();
	}

	JECommandPool::~JECommandPool()
	{
		vkDestroyCommandPool(_jeDevice.device(), _commandPool, nullptr);
	}

	void JECommandPool::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = _jeDevice.findQueueFamilies();

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

		if (vkCreateCommandPool(_jeDevice.device(), &commandPoolCreateInfo, nullptr, &_commandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool");

		SDL_Log("[+] Created command pool");
	}
}