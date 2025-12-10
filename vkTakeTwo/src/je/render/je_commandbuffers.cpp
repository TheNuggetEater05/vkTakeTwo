#include "je_commandbuffers.hpp"

#include <stdexcept>

namespace je
{
	JECommandBuffers::JECommandBuffers(
		JECommandPool& jeCommandPool,
		JERenderPass& jeRenderPass,
		JEFramebuffers& jeFramebuffers,
		JESwapchain& jeSwapchain,
		JEDevice& jeDevice,
		JEPipeline& jePipeline) : _jeCommandPool(jeCommandPool), _jeRenderPass(jeRenderPass), _jeFramebuffers(jeFramebuffers), _jeSwapchain(jeSwapchain), _jeDevice(jeDevice), _jePipeline(jePipeline)
	{
		createCommandBuffers();
	}

	JECommandBuffers::~JECommandBuffers()
	{

	}

	void JECommandBuffers::createCommandBuffers()
	{
		uint32_t bufferCount = _jeSwapchain.swapchainImages().size(); // MAYBE CHANGE IDK IF IT ERRORS YET
		_commandBuffers.resize(bufferCount);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = _jeCommandPool.commandPool();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = bufferCount;

		if (vkAllocateCommandBuffers(_jeDevice.device(), &commandBufferAllocateInfo, _commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers");

		SDL_Log("[+] Allocated command buffers");
	}

	void JECommandBuffers::recordCommandBuffer(uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &commandBufferBeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer");
	}
	void JECommandBuffers::beginRenderPass(uint32_t imageIndex)
	{
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = _jeRenderPass.renderPass();
		renderPassBeginInfo.framebuffer = _jeFramebuffers.framebuffers()[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = _jeSwapchain.swapchainExtent();
		
		VkClearValue clearColor = { { {0.1f, 0.1f, 0.1f, 1.0f} } };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(_commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _jePipeline.pipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_jeSwapchain.swapchainExtent().width);
		viewport.height = static_cast<float>(_jeSwapchain.swapchainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = _jeSwapchain.swapchainExtent();
		vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

		vkCmdDraw(_commandBuffers[imageIndex], 3, 1, 0, 0);
	}
	void JECommandBuffers::endRenderPass(uint32_t imageIndex)
	{
		vkCmdEndRenderPass(_commandBuffers[imageIndex]);
	}
	void JECommandBuffers::endCommandBuffer(uint32_t imageIndex)
	{
		vkEndCommandBuffer(_commandBuffers[imageIndex]);
	}
}