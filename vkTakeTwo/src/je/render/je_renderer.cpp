#include "je_renderer.hpp"

#include <stdexcept>
#include <glm/glm.hpp>

#include <array>

#include "../core/je_types.hpp"

const int MAX_FRAMES_IN_FLIGHT = 3;

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
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(_jeDevice.device(), _renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(_jeDevice.device(), _imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(_jeDevice.device(), _inFlightFences[i], nullptr);
		}
	}

	void JERenderer::setupSyncObjects()
	{
		_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(_jeDevice.device(), &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(_jeDevice.device(), &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(_jeDevice.device(), &fenceCreateInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create semaphores and fences");
		}
	}

	uint32_t JERenderer::startDraw()
	{
		vkWaitForFences(_jeDevice.device(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);	// Wait for previous frame to finish
		vkResetFences(_jeDevice.device(), 1, &_inFlightFences[_currentFrame]);							// Once previous frame finishes, unsignal the fence

		uint32_t imageIndex;
		vkAcquireNextImageKHR(_jeDevice.device(), _jeSwapchain->swapchain(), UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

		vkResetCommandBuffer(_jeCommandBuffers->commandBuffers()[imageIndex], 0); // Make sure our command buffer is ready to use

		_jeCommandBuffers->recordCommandBuffer(imageIndex);

		return imageIndex;
	}

	void JERenderer::endDraw(uint32_t imageIndex)
	{
		_jeCommandBuffers->endCommandBuffer(imageIndex);

		std::vector<VkSemaphore> waitSemaphores = { _imageAvailableSemaphores[_currentFrame] };
		std::vector<VkSemaphore> signalSemaphores = { _renderFinishedSemaphores[_currentFrame] };
		std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_jeCommandBuffers->commandBuffers()[imageIndex];

		if (vkQueueSubmit(_jeDevice.graphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = signalSemaphores.size();
		presentInfo.pWaitSemaphores = signalSemaphores.data();

		std::vector<VkSwapchainKHR> swapChains = { _jeSwapchain->swapchain() };
		presentInfo.swapchainCount = swapChains.size();
		presentInfo.pSwapchains = swapChains.data();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(_jeDevice.presentQueue(), &presentInfo);

		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	const std::vector<Vertex> vertices =
	{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	};

	void JERenderer::drawFrame()
	{
		uint32_t imageIndex = startDraw();

		vkCmdDraw(_jeCommandBuffers->commandBuffers()[imageIndex], 3, 1, 0, 0); // Draw call

		endDraw(imageIndex);
	}
}