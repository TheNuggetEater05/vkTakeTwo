#include "je_renderer.hpp"

#include <stdexcept>

namespace je
{
	JERenderer::JERenderer(JEDevice& device, JEWindow& window) : _jeDevice(device), _jeWindow(window)
	{
		_jeSwapchain = std::make_unique<JESwapchain>(device, window);
		_jeRenderPass = std::make_unique<JERenderPass>(*_jeSwapchain, _jeDevice);
		_jeFramebuffers = std::make_unique<JEFramebuffers>(*_jeSwapchain, *_jeRenderPass, _jeDevice);
		_jeCommandPool = std::make_unique<JECommandPool>(_jeDevice);
		_jePipeline = std::make_unique<JEPipeline>(_jeDevice, *_jeSwapchain, *_jeRenderPass);
		_jeCommandBuffers = std::make_unique<JECommandBuffers>(*_jeCommandPool, *_jeRenderPass, *_jeFramebuffers, *_jeSwapchain, _jeDevice, *_jePipeline);
		

		setupSyncObjects();
	}

	JERenderer::~JERenderer()
	{
		vkDestroySemaphore(_jeDevice.device(), _imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(_jeDevice.device(), _renderFinishedSemaphore, nullptr);
		vkDestroyFence(_jeDevice.device(), _inFlightFence, nullptr);
	}
	void JERenderer::setupSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(_jeDevice.device(), &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(_jeDevice.device(), &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateFence(_jeDevice.device(), &fenceCreateInfo, nullptr, &_inFlightFence) != VK_SUCCESS)
			throw std::runtime_error("failed to create semaphores!");
	}

	void JERenderer::drawFrame()
	{
		vkWaitForFences(_jeDevice.device(), 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(_jeDevice.device(), 1, &_inFlightFence);

		uint32_t imageIndex = 0;
		vkAcquireNextImageKHR(_jeDevice.device(), _jeSwapchain.get()->swapchain(), UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		vkResetCommandBuffer(_jeCommandBuffers.get()->commandBuffers()[imageIndex], 0);
		_jeCommandBuffers.get()->recordCommandBuffer(imageIndex);
		_jeCommandBuffers.get()->beginRenderPass(imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_jeCommandBuffers.get()->commandBuffers()[imageIndex];

		VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		_jeCommandBuffers.get()->endRenderPass(imageIndex);
		_jeCommandBuffers.get()->endCommandBuffer(imageIndex);

		if (vkQueueSubmit(_jeDevice.graphicsQueue(), 1, &submitInfo, _inFlightFence) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { _jeSwapchain.get()->swapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(_jeDevice.presentQueue(), &presentInfo);

		//vkEndCommandBuffer(_jeCommandBuffers.get()->commandBuffers()[imageIndex]);
	}
}