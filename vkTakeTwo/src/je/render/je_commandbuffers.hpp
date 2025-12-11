#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL_log.h>

#include <vector>

#include "je_commandpool.hpp"
#include "je_renderpass.hpp"
#include "je_framebuffers.hpp"
#include "je_swapchain.hpp"
#include "je_pipeline.hpp"

namespace je
{
	class JECommandBuffers
	{
	public:
		JECommandBuffers(JECommandPool& jeCommandPool, JERenderPass& jeRenderPass, JEFramebuffers& jeFramebuffers, JESwapchain& jeSwapchain, JEDevice& jeDevice, JEPipeline& jePipeline);
		~JECommandBuffers();

		JECommandBuffers(const JECommandBuffers&) = delete;
		JECommandBuffers& operator= (const JECommandBuffers&) = delete;

		std::vector<VkCommandBuffer>& commandBuffers() { return _commandBuffers; };
		void recordCommandBuffer(uint32_t imageIndex);
		void endCommandBuffer(uint32_t imageIndex);
	private:
		JECommandPool& _jeCommandPool;
		JERenderPass& _jeRenderPass;
		JEFramebuffers& _jeFramebuffers;
		JESwapchain& _jeSwapchain;
		JEDevice& _jeDevice;
		JEPipeline& _jePipeline;

		std::vector<VkCommandBuffer> _commandBuffers;

		void createCommandBuffers();
		
	};
}