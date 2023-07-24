#include "..\\framework\\application.h"

class CSimpleTriangle: public CApplication{
public:
    CSimpleTriangle(){
		/************
		Vertex是5个float*4字节=20
		vertices.size()是4
		buffer大小: 80（byte）
		************/
		vertices3D = {
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
			{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
			{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
			{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
		};

		/************
		buffer大小: 2*6=12（byte）
		************/
		indices3D = { 0, 1, 2, 2, 3, 0};

		vertexShaderPath = "../shaders/basic/vert.spv";
    	fragmentShaderPath = "../shaders/basic/frag.spv";
    	//computeShaderPath = "";
    }

    ~CSimpleTriangle(){

    }

	void updateUniformBuffer(uint32_t currentFrame, float durationTime) {
		//printf("update uniform in triangle...\n");

		UniformBufferObject ubo{};
		//ubo.view = camera.matrices.view;
		//ubo.proj = camera.matrices.perspective;
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4x4 m = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10.0f);
		m[1][1] *= -1;
		ubo.proj = m;
		// if (pipelineType == PIPELINE_MIPMAP) {
		//     ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		// }
		// else {
			ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//}

		memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

		CApplication::updateUniformBuffer(currentFrame, durationTime);
	}

	void update(){
		//printf("triangle update...\n");

		CApplication::update();
	}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		CApplication::recordCommandBuffer(commandBuffer, imageIndex);
	}

	void drawFrame(){
		//printf("triangle drawFrame...\n");

		CApplication::drawFrame();
	}

	void initVulkan(){

		CApplication::initVulkan();
	}

	void Init10CreateRenderPass() {
		HERE_I_AM("Init10CreateRenderPass");

		VkResult result = VK_SUCCESS;

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;//VK_FORMAT_B8G8R8A8_SRGB
		//colorAttachment.samples = bEnableMSAA ? msaaSamples : VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//colorAttachment.finalLayout = bEnableMSAA ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		//added for model
		// VkAttachmentDescription depthAttachment{};
		// depthAttachment.format = findDepthFormat();
		// depthAttachment.samples = bEnableMSAA ? msaaSamples : VK_SAMPLE_COUNT_1_BIT;
		// depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		// depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		//added for MSAA
		// VkAttachmentDescription colorAttachmentResolve{};
		// colorAttachmentResolve.format = swapChainImageFormat;
		// colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		// colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		// colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		// colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//added for model
		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//added for MSAA
		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		//if (bEnableDepthTest) subpass.pDepthStencilAttachment = &depthAttachmentRef; //added for model
		//if (bEnableMSAA) subpass.pResolveAttachments = &colorAttachmentResolveRef; //added for MSAA

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		// if (bEnableDepthTest) {
		//     dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		//     dependency.srcAccessMask = 0;
		//     dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		//     dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		// }
		// else {
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//}

		std::vector<VkAttachmentDescription> attachments;
		attachments.push_back(colorAttachment);
		//if (bEnableDepthTest) attachments.push_back(depthAttachment);
		//if (bEnableMSAA) attachments.push_back(colorAttachmentResolve);

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data(); //结构1
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;//结构2
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;//结构3

		result = vkCreateRenderPass(LOGICAL_DEVICE, &renderPassInfo, nullptr, &renderPass);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");
		REPORT("vkCreateRenderPass");
	}

	void Init11CreateFramebuffers() {
		HERE_I_AM("Init11CreateFramebuffers");

		VkResult result = VK_SUCCESS;

		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::vector<VkImageView> attachments;
			// if (bEnableMSAA) {
			//     attachments.push_back(colorImageView_msaa);//for MSAA
			//     attachments.push_back(depthImageView);//for model
			//     attachments.push_back(swapChainImageViews[i]);
			// }
			// else {
				attachments.push_back(swapChainImageViews[i]);
				//attachments.push_back(depthImageView);//for model
			//}

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();//swapChainImageViews, 类型是VkImageView
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			result = vkCreateFramebuffer(LOGICAL_DEVICE, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
			if (result != VK_SUCCESS) throw std::runtime_error("failed to create framebuffer!");
			REPORT("vkCreateFrameBuffer");
		}
	}

	void Init13CreateDescriptorPool(VkDescriptorPool &descriptorPool, PipelineType pt) {
		HERE_I_AM("Init13DescriptorSetPool");

		VkResult result = VK_SUCCESS;

		std::vector<VkDescriptorPoolSize> poolSizes;
		switch (pt) {
		case PIPELINE_BASIC:
			poolSizes.resize(1);
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
			break;
		// case PIPELINE_TEXTURE:
		// case PIPELINE_MODEL:
		// case PIPELINE_MIPMAP:
		//     poolSizes.resize(2);
		//     poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//     poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		//     poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//     poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		//     break;
		// case PIPELINE_COMPUTE:
		//     poolSizes.resize(2);
		//     poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//     poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		//     poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		//     poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;
		//     break;
		default:
			break;
		}

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		//Step 1
		result = vkCreateDescriptorPool(LOGICAL_DEVICE, &poolInfo, nullptr, &descriptorPool);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
		REPORT("vkCreateDescriptorPool")
	}

	void Init13CreateDescriptorSetLayout(VkDescriptorSetLayout &descriptorSetLayout, PipelineType pt) {
		HERE_I_AM("Init13CreateDescriptorSetLayout");

		VkResult result = VK_SUCCESS;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		std::vector<VkDescriptorSetLayoutBinding> bindings;
		bindings.push_back(uboLayoutBinding);

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};

		// switch (pt) {
		// case PIPELINE_BASIC:
		//     break;
		// case PIPELINE_TEXTURE:
		// case PIPELINE_MODEL:
		// case PIPELINE_MIPMAP:
		//     samplerLayoutBinding.binding = 1;
		//     samplerLayoutBinding.descriptorCount = 1;
		//     samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//     samplerLayoutBinding.pImmutableSamplers = nullptr;
		//     samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		//     bindings.push_back(samplerLayoutBinding);
		//     break;
		// default:
		//     break;
		// }

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		//Step 2
		result = vkCreateDescriptorSetLayout(LOGICAL_DEVICE, &layoutInfo, nullptr, OUT &descriptorSetLayout);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
		REPORT("vkCreateDescriptorSetLayout");
	}

	void Init13CreateDescriptorSets(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkDescriptorSet> &descriptorSets, PipelineType pt) {
		HERE_I_AM("Init13CreateDescriptorSets");

		VkResult result = VK_SUCCESS;

		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		//Step 3
		result = vkAllocateDescriptorSets(LOGICAL_DEVICE, &allocInfo, descriptorSets.data());
		if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");
		REPORT("vkAllocateDescriptorSets");

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			std::vector<VkWriteDescriptorSet> descriptorWrites;
			VkDescriptorBufferInfo bufferInfo{};
			VkDescriptorImageInfo imageInfo{};

			VkDescriptorBufferInfo storageBufferInfoLastFrame{}; //for compute shader
			VkDescriptorBufferInfo storageBufferInfoCurrentFrame{}; //for compute shader

			switch (pt) {
			case PIPELINE_BASIC:
				descriptorWrites.resize(1);

				bufferInfo.buffer = uniformBuffers[i].buffer;
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;
				break;
			// case PIPELINE_TEXTURE:
			// case PIPELINE_MODEL:
			// case PIPELINE_MIPMAP:
			//     descriptorWrites.resize(2);

			//     bufferInfo.buffer = uniformBuffers[i].buffer;
			//     bufferInfo.offset = 0;
			//     bufferInfo.range = sizeof(UniformBufferObject);
			//     descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//     descriptorWrites[0].dstSet = descriptorSets[i];
			//     descriptorWrites[0].dstBinding = 0;
			//     descriptorWrites[0].dstArrayElement = 0;
			//     descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//     descriptorWrites[0].descriptorCount = 1;
			//     descriptorWrites[0].pBufferInfo = &bufferInfo;

			//     imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			//     imageInfo.imageView = textureImageView[pt];
			//     imageInfo.sampler = textureSampler;
			//     descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//     descriptorWrites[1].dstSet = descriptorSets[i];
			//     descriptorWrites[1].dstBinding = 1;
			//     descriptorWrites[1].dstArrayElement = 0;
			//     descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//     descriptorWrites[1].descriptorCount = 1;
			//     descriptorWrites[1].pImageInfo = &imageInfo;
			//     break;
			// case PIPELINE_COMPUTE:
			//     descriptorWrites.resize(3);

			//     //VkDescriptorBufferInfo uniformBufferInfo{};
			//     bufferInfo.buffer = uniformBuffers_compute[i].buffer;
			//     bufferInfo.offset = 0;
			//     bufferInfo.range = sizeof(UniformBufferObject_compute);

			//     //std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
			//     descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//     descriptorWrites[0].dstSet = descriptorSets[i];
			//     descriptorWrites[0].dstBinding = 0;
			//     descriptorWrites[0].dstArrayElement = 0;
			//     descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//     descriptorWrites[0].descriptorCount = 1;
			//     descriptorWrites[0].pBufferInfo = &bufferInfo;

			//     //VkDescriptorBufferInfo storageBufferInfoLastFrame{};
			//     storageBufferInfoLastFrame.buffer = shaderStorageBuffers_compute[(i - 1) % MAX_FRAMES_IN_FLIGHT].buffer;
			//     storageBufferInfoLastFrame.offset = 0;
			//     storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

			//     descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//     descriptorWrites[1].dstSet = descriptorSets[i];
			//     descriptorWrites[1].dstBinding = 1;
			//     descriptorWrites[1].dstArrayElement = 0;
			//     descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			//     descriptorWrites[1].descriptorCount = 1;
			//     descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

			//     //VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
			//     storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers_compute[i].buffer;
			//     storageBufferInfoCurrentFrame.offset = 0;
			//     storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

			//     descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//     descriptorWrites[2].dstSet = descriptorSets[i];
			//     descriptorWrites[2].dstBinding = 2;
			//     descriptorWrites[2].dstArrayElement = 0;
			//     descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			//     descriptorWrites[2].descriptorCount = 1;
			//     descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;
			//     break;
			default:
				break;
			}

			//Step 4
			vkUpdateDescriptorSets(LOGICAL_DEVICE, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			//vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrites[0], 0, nullptr);

		}
	}

	void Init14CreateGraphicsPipeline(VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, VkDescriptorSetLayout &descriptorSetLayout, VkPipelineLayout &pipelineLayout, VkPipeline &graphicsPipeline, PipelineType pt){
		HERE_I_AM("Init14CreateGraphicsPipeline");

		VkResult result = VK_SUCCESS;

		//1 组装Shader
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		//2 组装Vertex Info
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		//NTODO: particle and vertex3d
		// if (pt == PIPELINE_COMPUTE) {
		// 	auto bindingDescription = Particle::getBindingDescription();
		// 	auto attributeDescriptions = Particle::getAttributeDescriptions();

		// 	vertexInputInfo.vertexBindingDescriptionCount = 1;
		// 	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		// 	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		// 	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		// }
		// else {
			auto bindingDescription = Vertex3D::getBindingDescription();
			auto attributeDescriptions = Vertex3D::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		//}

		//3 组装拓扑结构
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = topology;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		//4 组装Viewport
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		//5 组装光栅化信息
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		//6 组装MSAA组件
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		//multisampling.rasterizationSamples = bEnableMSAA ? msaaSamples : VK_SAMPLE_COUNT_1_BIT;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		//7 组装color blend组件
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		// if (pt == PIPELINE_COMPUTE) { //blend particle color in compute pipeline
		// 	colorBlendAttachment.blendEnable = VK_TRUE;
		// 	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		// 	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		// 	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		// 	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		// 	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		// 	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		// }
		// else {
			colorBlendAttachment.blendEnable = VK_FALSE;
		//}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		//8 组装dynamicState
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		//9 组装Pipeline Layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// if (pt == PIPELINE_COMPUTE) {
		// 	pipelineLayoutInfo.setLayoutCount = 0;
		// 	pipelineLayoutInfo.pSetLayouts = nullptr;
		// }else {
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		//}

		result = vkCreatePipelineLayout(LOGICAL_DEVICE, &pipelineLayoutInfo, nullptr, &pipelineLayout);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout!");
		REPORT("vkCreatePipelineLayout");

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;	//1 组装Shader
		pipelineInfo.pVertexInputState = &vertexInputInfo;	//2 组装Vertex Info
		pipelineInfo.pInputAssemblyState = &inputAssembly;	//3 组装拓扑结构
		pipelineInfo.pViewportState = &viewportState;	//4 组装Viewport
		pipelineInfo.pRasterizationState = &rasterizer;	//5 组装光栅化信息
		pipelineInfo.pMultisampleState = &multisampling;	//6 组装MSAA组件

		// if (bEnableDepthTest) {
		// 	VkPipelineDepthStencilStateCreateInfo depthStencil{};
		// 	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		// 	depthStencil.depthTestEnable = VK_TRUE;
		// 	depthStencil.depthWriteEnable = VK_TRUE;
		// 	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		// 	depthStencil.depthBoundsTestEnable = VK_FALSE;
		// 	depthStencil.stencilTestEnable = VK_FALSE;
		// 	pipelineInfo.pDepthStencilState = &depthStencil;
		// }

		pipelineInfo.pColorBlendState = &colorBlending;	//7 组装color blend组件
		pipelineInfo.pDynamicState = &dynamicState;	//8 组装dynamicState
		pipelineInfo.layout = pipelineLayout;	//9 组装Pipeline Layout
		pipelineInfo.renderPass = renderPass;	//10 组装RenderPass
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		result = vkCreateGraphicsPipelines(LOGICAL_DEVICE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");
		REPORT("vkCreateGraphicsPipelines");

		vkDestroyShaderModule(LOGICAL_DEVICE, fragShaderModule, nullptr);
		vkDestroyShaderModule(LOGICAL_DEVICE, vertShaderModule, nullptr);

		HERE_I_AM("DrawFrame() will begin");
	}

};

int main(){
    //CApplication app;
	CSimpleTriangle app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
    return 0;
}