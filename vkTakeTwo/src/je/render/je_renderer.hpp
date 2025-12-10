#pragma once

#include <memory>

#include "je_swapchain.hpp"
#include "je_renderpass.hpp"
#include "je_framebuffers.hpp"
#include "je_commandpool.hpp"
#include "je_commandbuffers.hpp"
#include "je_pipeline.hpp"

namespace je
{
	class JERenderer
	{
	public:
		JERenderer(JEDevice& device, JEWindow& window);
		~JERenderer();

		JERenderer(const JERenderer&) = delete;
		JERenderer& operator= (const JERenderer&) = delete;

		void drawFrame();
	private:
		std::unique_ptr<JESwapchain> _jeSwapchain;
		std::unique_ptr<JERenderPass> _jeRenderPass;
		std::unique_ptr<JEFramebuffers> _jeFramebuffers;
		std::unique_ptr<JECommandPool> _jeCommandPool;
		std::unique_ptr<JECommandBuffers> _jeCommandBuffers;
		std::unique_ptr<JEPipeline> _jePipeline;

		JEDevice& _jeDevice;
		JEWindow& _jeWindow;

		VkSemaphore _imageAvailableSemaphore;
		VkSemaphore _renderFinishedSemaphore;
		VkFence _inFlightFence;

		void setupSyncObjects();

		
	};
}