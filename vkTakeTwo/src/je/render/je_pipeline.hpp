#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL_log.h>

#include <vector>

#include "../core/je_device.hpp"
#include "je_swapchain.hpp"
#include "je_renderpass.hpp"

namespace je
{
	class JEPipeline
	{
	public:
		JEPipeline(JEDevice& jeDevice, JESwapchain& jeSwapchain, JERenderPass& jeRenderPass);
		~JEPipeline();

		JEPipeline(const JEPipeline&) = delete;
		JEPipeline& operator= (const JEPipeline&) = delete;

		VkPipelineLayout& pipelineLayout() { return _pipelineLayout; };
		VkPipeline& pipeline() { return _pipeline; };

	private:
		JEDevice& _jeDevice;
		JESwapchain& _jeSwapchain;
		JERenderPass& _jeRenderPass;

		VkShaderModule _vertShaderModule;
		VkShaderModule _fragShaderModule;

		VkPipelineLayout _pipelineLayout;
		VkPipeline _pipeline;

		void createPipeline();
		VkShaderModule createShaderModule(std::vector<char>& code);

		std::vector<char> readFile(const char* filename) const;
	};
}