#include "vulkanBase.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

CVulkanBase::CVulkanBase(){
    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 20.0f;
}

CVulkanBase::~CVulkanBase(){  
}

void CVulkanBase::wxjCreateColorAttachment(){  
	bUseColorAttachment = true;

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
}
void CVulkanBase::wxjCreatDepthAttachment(){  
	bUseDepthAttachment = true;

	//added for model
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	//depthAttachment.samples = bEnableMSAA ? msaaSamples : VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
}
void CVulkanBase::wxjCreatColorAttachmentResolve(){  
	bUseColorAttachmentResolve = true;

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
}
void CVulkanBase::wxjCreateSubpass(){ 
	uint32_t attachmentCount = 0;
	if(bUseColorAttachment){
		colorAttachmentRef.attachment = attachmentCount++;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &colorAttachmentRef;
	}
	if(bUseDepthAttachment){
		//added for model
		depthAttachmentRef.attachment = attachmentCount++;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		subpass.pDepthStencilAttachment = &depthAttachmentRef; //added for model
	}
	if(bUseColorAttachmentResolve){
		//added for MSAA
		//colorAttachmentResolveRef.attachment = attachmentCount++;
		//colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//subpass.pResolveAttachments = &colorAttachmentResolveRef; //added for MSAA
	}
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
}
void CVulkanBase::wxjCreateDependency(VkPipelineStageFlags srcPipelineStageFlag, VkPipelineStageFlags dstPipelineStageFlag){  
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = srcPipelineStageFlag;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = dstPipelineStageFlag;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (bUseDepthAttachment) 
		dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
}
void CVulkanBase::wxjCreateRenderPass(){ 
	HERE_I_AM("wxjCreateRenderPass");

	VkResult result = VK_SUCCESS;

	std::vector<VkAttachmentDescription> attachments;
	if(bUseColorAttachment) attachments.push_back(colorAttachment);
	if(bUseDepthAttachment) attachments.push_back(depthAttachment);
	if(bUseColorAttachmentResolve) attachments.push_back(colorAttachmentResolve);

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

void CVulkanBase::wxjCreateFramebuffers(){
	HERE_I_AM("wxjCreateFramebuffers");

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
			attachments.push_back(depthImageView);//for model
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

void CVulkanBase::wxjCreateDescriptorPool(std::vector<VkDescriptorType> &descriptorTypes){
	HERE_I_AM("wxjCreateDescriptorPool");

	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.resize(descriptorTypes.size());
	
	for(int i = 0; i < descriptorTypes.size(); i++){
		poolSizes[i].type = descriptorTypes[i];
		poolSizes[i].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	}
	//std::vector<VkDescriptorPoolSize> poolSizes;
	// switch (pt) {
	// case PIPELINE_BASIC:
	// 	poolSizes.resize(1);
	// 	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// 	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	// 	break;
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
	//default:
	//	break;
	//}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	//Step 1
	VkResult result = vkCreateDescriptorPool(LOGICAL_DEVICE, &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
	REPORT("vkCreateDescriptorPool");
}
void CVulkanBase::wxjCreateDescriptorSetLayout(std::vector<VkDescriptorType> &descriptorTypes, std::vector<VkShaderStageFlagBits>& shaderStageFlagBits){
	HERE_I_AM("wxjCreateDescriptorSetLayout");

	//VkDescriptorSetLayoutBinding uboLayoutBinding{};
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.resize(descriptorTypes.size());
	for(int i = 0; i < descriptorTypes.size(); i++){
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].descriptorType = descriptorTypes[i];
		bindings[i].pImmutableSamplers = nullptr;
		bindings[i].stageFlags = shaderStageFlagBits[i];
	}


	// uboLayoutBinding.binding = 0;
	// uboLayoutBinding.descriptorCount = 1;
	// uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// uboLayoutBinding.pImmutableSamplers = nullptr;
	// uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//std::vector<VkDescriptorSetLayoutBinding> bindings;
	//bindings.push_back(uboLayoutBinding);

	//VkDescriptorSetLayoutBinding samplerLayoutBinding{};

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
	VkResult result = vkCreateDescriptorSetLayout(LOGICAL_DEVICE, &layoutInfo, nullptr, OUT &descriptorSetLayout);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	REPORT("vkCreateDescriptorSetLayout");
}
void CVulkanBase::wxjCreateDescriptorSets(std::vector<VkDescriptorType> &descriptorTypes){
	HERE_I_AM("wxjCreateDescriptorSets");

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

		descriptorWrites.resize(descriptorTypes.size());
		for(int j = 0; j < descriptorTypes.size(); j++){
			if(descriptorTypes[j] == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
				bufferInfo.buffer = uniformBuffers[i].buffer;
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);
				descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[j].dstSet = descriptorSets[i];
				descriptorWrites[j].dstBinding = j;
				descriptorWrites[j].dstArrayElement = 0;
				descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[j].descriptorCount = 1;
				descriptorWrites[j].pBufferInfo = &bufferInfo;
			}

			if(descriptorTypes[j] == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textureImageView;
				imageInfo.sampler = textureSampler;
				descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[j].dstSet = descriptorSets[i];
				descriptorWrites[j].dstBinding = j;
				descriptorWrites[j].dstArrayElement = 0;
				descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[j].descriptorCount = 1;
				descriptorWrites[j].pImageInfo = &imageInfo;
			}
		}

		/*switch (pt) {
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
		}*/

		//Step 4
		vkUpdateDescriptorSets(LOGICAL_DEVICE, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		//vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrites[0], 0, nullptr);

	}
}

void CVulkanBase::wxjCreateGraphicsPipeline(VkPrimitiveTopology topology){
	HERE_I_AM("wxjCreateGraphicsPipeline");

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

	if (bUseDepthAttachment) {
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		pipelineInfo.pDepthStencilState = &depthStencil;
	}

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

void CVulkanBase::wxjCreateVertexShader(std::string shaderName){
    Init12SpirvShader(shaderName, &vertShaderModule);
}
void CVulkanBase::wxjCreateFragmentShader(std::string shaderName){
	Init12SpirvShader(shaderName, &fragShaderModule);
}

void CVulkanBase::wxjCreateSyncObjects(){
    createSyncObjects();
}

void CVulkanBase::wxjCreateCommandBuffer(){
	Init06CreateCommandPool();
	Init06CreateCommandBuffers();
}
void CVulkanBase::wxjCreateVertexBuffer(){
    Init05CreateVertexBuffer();
}
void CVulkanBase::wxjCreateIndexBuffer(){
    Init05CreateIndexBuffer();
}
void CVulkanBase::wxjCreateUniformBuffers(){
    Init05CreateUniformBuffers(uniformBuffers, uniformBuffersMapped, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject));
}

void CVulkanBase::wxjBeginCommandBuffer(){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}
void CVulkanBase::wxjBeginRenderPass(std::vector<VkClearValue> &clearValues0){
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    // if (bEnableDepthTest) {
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
    // }
    // else {
    	//clearValues[0].color = { {  0.0f, 0.0f, 0.0f, 1.0f  } };
    	//renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    	//renderPassInfo.pClearValues = clearValues.data();
    //}

    //Step2
    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void CVulkanBase::wxjBindPipeline(){
	vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}
void CVulkanBase::wxjSetViewport(){
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	//Step4
	vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);
}
void CVulkanBase::wxjSetScissor(){
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
}
void CVulkanBase::wxjBindVertexBuffer(){
	VkBuffer vertexBuffers[] = { vertexDataBuffer.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
}
void CVulkanBase::wxjBindIndexBuffer(){
	vkCmdBindIndexBuffer(commandBuffers[currentFrame], indexDataBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}
void CVulkanBase::wxjBindDescriptorSets(){
	vkCmdBindDescriptorSets(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
}
void CVulkanBase::wxjDrawIndexed(){
	vkCmdDrawIndexed(commandBuffers[currentFrame], static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
}
void CVulkanBase::wxjEndRenderPass(){
	vkCmdEndRenderPass(commandBuffers[currentFrame]);
}
void CVulkanBase::wxjEndCOmmandBuffer(){
	if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CVulkanBase::wxjCreateImage_texture(const std::string texturePath, OUT MyImageBuffer &textureImageBuffer, OUT int32_t &texWidth, OUT int32_t &texHeight) {
	HERE_I_AM("Init07CreateTextureImage");

	int texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;


	//mipLevels = bEnableMipMap ? (static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1) : 1;
    int mipLevels = 1;


	if (!pixels) throw std::runtime_error("failed to load texture image!");


	MyBuffer stagingBuffer;
	VkResult result = InitDataBufferHelper(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &stagingBuffer);
	REPORT("InitTextureImageBuffer");
	//FillDataBufferHelper(stagingBuffer, (void *)(vertices.data()));

	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	FillDataBufferHelper(stagingBuffer, pixels);
	//void* data;
	//vkMapMemory(logicalDevice, stagingBuffer.deviceMemory, 0, imageSize, 0, &data);
	//memcpy(data, pixels, static_cast<size_t>(imageSize));
	//vkUnmapMemory(logicalDevice, stagingBuffer.deviceMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImageBuffer);

	transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
	copyBufferToImage(stagingBuffer.buffer, textureImageBuffer.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	
	//MIPMAP TODO: no need transition??
	//transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	vkDestroyBuffer(LOGICAL_DEVICE, stagingBuffer.buffer, nullptr);
	vkFreeMemory(LOGICAL_DEVICE, stagingBuffer.deviceMemory, nullptr);
}
void CVulkanBase::wxjCreateSampler_texture() {
	HERE_I_AM("Init07CreateTextureSampler");

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(PHYSICAL_DEVICE, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	
	// if (bEnableMipMap) {
	// 	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;// VK_SAMPLER_MIPMAP_MODE_NEAREST;
	// 	samplerInfo.minLod = 0.0f;
	// 	samplerInfo.maxLod = static_cast<float>(mipLevels / 2);
	// 	samplerInfo.mipLodBias = 0.0f;
	// }

	VkResult result = vkCreateSampler(LOGICAL_DEVICE, &samplerInfo, nullptr, &textureSampler);
	REPORT("vkCreateSampler");
}
void CVulkanBase::wxjCreateImageView(IN VkImage image, OUT VkImageView &imageView, VkFormat format, VkImageAspectFlags aspectFlags){
    imageView = createImageView(image, format, aspectFlags, 1);
}
void CVulkanBase::wxjCreateImage(OUT MyImageBuffer &imageBuffer, VkFormat format){
	int mipLevels = 1;//TODO
	createImage(swapChainExtent.width, swapChainExtent.height, mipLevels, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageBuffer);
}
void CVulkanBase::wxjCreateSwapChain(){
    Init08CreateSwapChain();
}

void CVulkanBase::wxjLoadObjModel(const std::string modelName) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelName.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};

	vertices3D.clear();
	indices3D.clear();

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex3D vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices3D.size());
				vertices3D.push_back(vertex);
			}

			indices3D.push_back(uniqueVertices[vertex]);
		}
	}
}




