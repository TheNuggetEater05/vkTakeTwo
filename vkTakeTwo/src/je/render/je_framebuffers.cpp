#include "je_framebuffers.hpp"

#include <stdexcept>

namespace je
{
	JEFramebuffers::JEFramebuffers(JESwapchain& jeSwapchain, JERenderPass& jeRenderPass, JEDevice& jeDevice) : _jeSwapchain(jeSwapchain), _jeRenderPass(jeRenderPass), _jeDevice(jeDevice)
	{
		createFramebuffers();
	}

	JEFramebuffers::~JEFramebuffers()
	{
		for (auto framebuffer : _framebuffers)
			vkDestroyFramebuffer(_jeDevice.device(), framebuffer, nullptr);
	}

	void JEFramebuffers::createFramebuffers()
	{
		SDL_Log("Starting createFramebuffers %u", _jeSwapchain.swapchainImages().size());
		_framebuffers.resize(_jeSwapchain.swapchainImageViews().size());

		for (uint32_t i = 0; i < _jeSwapchain.swapchainImageViews().size(); i++)
		{
			VkImageView attachments[] =
			{
				_jeSwapchain.swapchainImageViews()[i]
			};

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = _jeRenderPass.renderPass();
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = attachments;
			framebufferCreateInfo.width = _jeSwapchain.swapchainExtent().width;
			framebufferCreateInfo.height = _jeSwapchain.swapchainExtent().height;
			framebufferCreateInfo.layers = 1;

			SDL_Log("[+] Created framebuffer %u", i);
			if (vkCreateFramebuffer(_jeDevice.device(), &framebufferCreateInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer");
		}
	}
}