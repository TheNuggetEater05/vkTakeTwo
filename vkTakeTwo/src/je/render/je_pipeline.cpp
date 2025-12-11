#include "je_pipeline.hpp"

#include <fstream>

#include "../core/je_types.hpp"

namespace je
{
	JEPipeline::JEPipeline(JEDevice& jeDevice, JESwapchain& jeSwapchain, JERenderPass& jeRenderPass) : _jeDevice(jeDevice), _jeSwapchain(jeSwapchain), _jeRenderPass(jeRenderPass)
	{
		createPipeline();
	}

	JEPipeline::~JEPipeline()
	{
		vkDestroyShaderModule(_jeDevice.device(), _vertShaderModule, nullptr);
		vkDestroyShaderModule(_jeDevice.device(), _fragShaderModule, nullptr);

		vkDestroyPipeline(_jeDevice.device(), _pipeline, nullptr);
		vkDestroyPipelineLayout(_jeDevice.device(), _pipelineLayout, nullptr);
	}

	void JEPipeline::createPipeline()
	{
		auto vertShaderCode = readFile("resources/shaders/simple.vert.spv");
		auto fragShaderCode = readFile("resources/shaders/simple.frag.spv");

		_vertShaderModule = createShaderModule(vertShaderCode);
		_fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
		vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageCreateInfo.module = _vertShaderModule;
		vertShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
		fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfo.module = _fragShaderModule;
		fragShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = _jeSwapchain.swapchainExtent().width;
		viewport.height = _jeSwapchain.swapchainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = _jeSwapchain.swapchainExtent();

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.lineWidth = 1.0f;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.minSampleShading = 1.0f;
		multisampleStateCreateInfo.pSampleMask = nullptr;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentState.blendEnable = VK_FALSE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.pSetLayouts = nullptr;
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(_jeDevice.device(), &layoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout");

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr;
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.layout = _pipelineLayout;
		pipelineCreateInfo.renderPass = _jeRenderPass.renderPass();
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(_jeDevice.device(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline");

		SDL_Log("[+] CREATED PIPELINE FINALLY");
	}

	VkShaderModule JEPipeline::createShaderModule(std::vector<char>& code)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = code.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;

		if (vkCreateShaderModule(_jeDevice.device(), &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");

		return shaderModule;
	}

	std::vector<char> JEPipeline::readFile(const char* filename) const
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Failed to open file");

		uint32_t fileSize = static_cast<uint32_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}