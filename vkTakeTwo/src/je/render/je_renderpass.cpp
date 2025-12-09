#include "je_renderpass.hpp"

#include <stdexcept>

namespace je
{
	JERenderPass::JERenderPass(JESwapchain& swapchain, JEDevice& device) : _jeSwapchain(swapchain), _jeDevice(device)
	{
		createRenderPass();
	}

	JERenderPass::~JERenderPass()
	{
		vkDestroyRenderPass(_jeDevice.device(), _renderPass, nullptr);
	}

	void JERenderPass::createRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = _jeSwapchain.swapchainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentReference{};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;

		SDL_Log("[+] Created renderpass!!");
		if (vkCreateRenderPass(_jeDevice.device(), &renderPassCreateInfo, nullptr, &_renderPass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create renderpass");
	}
}