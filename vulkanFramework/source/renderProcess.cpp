#include "../include/renderProcess.h"

void CRenderProcess::createSubpass_mainscene(int attachment_id_to_observe){ 
	if(iAttachmentDepthLight >= 0) clearValues.push_back({1.0f, 0}); 
	if(iAttachmentDepthCamera >= 0) clearValues.push_back({1.0f, 0}); 
	if(iAttachmentColorResovle >= 0) clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
	if(iAttachmentColorPresent >= 0) clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});

	if(bEnableSubpassShadowmap) createSubpass_mainscene_shadowmap();	
	if(bEnableSubpassDraw) createSubpass_mainscene_draw();	
	if(bEnableSubpassObserve) createSubpass_mainscene_observe(attachment_id_to_observe);	
}

void CRenderProcess::createSubpass_mainscene_shadowmap(){ //assume depth and MSAA are enabled
	VkSubpassDescription subpass{};//to generate light depth for shadowmap
	
	if(iAttachmentDepthLight >= 0){
		attachmentRef_light_depth_shadowmap.attachment = iAttachmentDepthLight; 
		attachmentRef_light_depth_shadowmap.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format
		subpass.pDepthStencilAttachment = &attachmentRef_light_depth_shadowmap;
	}

	subpass.colorAttachmentCount = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
	subpasses_mainscene.push_back(subpass);
}

void CRenderProcess::createSubpass_mainscene_draw(){ 
	VkSubpassDescription subpass{};

	// if(iAttachmentDepthLight >= 0 && iAttachmentDepthCamera >= 0){
	// 	attachmentRef_input_draw[0].attachment = iAttachmentDepthCamera;
	// 	attachmentRef_input_draw[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	// 	attachmentRef_input_draw[1].attachment = iAttachmentDepthLight;
	// 	subpass.pInputAttachments = attachmentRef_input_draw.data();
	// 	subpass.inputAttachmentCount = 2;
	// }

	if(iAttachmentDepthLight >= 0){
		attachmentRef_input_draw[0].attachment = iAttachmentDepthLight;
		attachmentRef_input_draw[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		subpass.pInputAttachments = &attachmentRef_input_draw[0];
		subpass.inputAttachmentCount = 1;
	}

	if(iAttachmentDepthCamera >= 0){
		attachmentRef_depth_draw.attachment = iAttachmentDepthCamera; 
		attachmentRef_depth_draw.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format: for shadowmapping, this is input and output
		subpass.pDepthStencilAttachment = &attachmentRef_depth_draw;
	}
	if(iAttachmentColorPresent >= 0){
		attachmentRef_color_draw.attachment = iAttachmentColorPresent; 
		attachmentRef_color_draw.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_color_draw;
		subpass.colorAttachmentCount = 1;
	}
	if(iAttachmentColorResovle >= 0){
		attachmentRef_color_multisample_draw.attachment = iAttachmentColorResovle; 
		attachmentRef_color_multisample_draw.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_color_multisample_draw; //use multi sampler color attachment to override pColorAttachments
		subpass.pResolveAttachments = &attachmentRef_color_draw;
	}

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
	subpasses_mainscene.push_back(subpass);
}

void CRenderProcess::createSubpass_mainscene_observe(int attachment_id_to_observe){ 
	VkSubpassDescription subpass{};

	attachmentRef_observe.attachment = attachment_id_to_observe; //iAttachmentDepthCamera or iAttachmentDepthLight
	attachmentRef_observe.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; //depth image format for output
	subpass.pInputAttachments = &attachmentRef_observe;
	subpass.inputAttachmentCount = 1;

	if(iAttachmentColorPresent >= 0){
		attachmentRef_color_observe.attachment = iAttachmentColorPresent; 
		attachmentRef_color_observe.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_color_observe;
		subpass.colorAttachmentCount = 1;
	}
	if(iAttachmentColorResovle >= 0){
		attachmentRef_color_multisample_observe.attachment = iAttachmentColorResovle; 
		attachmentRef_color_multisample_observe.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_color_multisample_observe;
		subpass.pResolveAttachments = &attachmentRef_color_observe;
	}

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses_mainscene.push_back(subpass);
}


void CRenderProcess::createDependency_mainscene(){  
	if(!bEnableSubpassShadowmap && bEnableSubpassDraw && !bEnableSubpassObserve){ //single subpass
		dependencies_mainscene.resize(1);

		//external->0
		if (iAttachmentDepthCamera < 0) {
			VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies_mainscene[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies_mainscene[0].dstSubpass = 0;
			dependencies_mainscene[0].srcStageMask = srcPipelineStageFlag;
			dependencies_mainscene[0].srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			dependencies_mainscene[0].dstStageMask = dstPipelineStageFlag;
			dependencies_mainscene[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}else{
			VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies_mainscene[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies_mainscene[0].dstSubpass = 0;
			dependencies_mainscene[0].srcStageMask = srcPipelineStageFlag;
			dependencies_mainscene[0].srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			dependencies_mainscene[0].dstStageMask = dstPipelineStageFlag;
			dependencies_mainscene[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			dependencies_mainscene[0].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
	}

	if(!bEnableSubpassShadowmap && bEnableSubpassDraw && bEnableSubpassObserve){ //two subpasses
		dependencies_mainscene.resize(2);

		//0->1
        dependencies_mainscene[0].srcSubpass = 0;//subpass0 is the src subpass, write in color image, output depth image
		dependencies_mainscene[0].dstSubpass = 1;//subpass1 is the dst subpass, input depth image, output color image(fragment shader convert depth image to color image)

		dependencies_mainscene[0].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this is to make sure subpass0 finishes depth/color, then begin subpass1
		dependencies_mainscene[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //subpass0 need write depth image, subpass1 cant not access depth image before subpass 0 finish it
		dependencies_mainscene[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //this is to say subpass1's fragment shader can read depth image that subpass1 wrote(as input attachment)
		dependencies_mainscene[0].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; //subpass1 need to read input attachment(that subpass0 generate)

		dependencies_mainscene[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; //sync region instead of all framebuffer, to improve performance
	}

	if(bEnableSubpassShadowmap && bEnableSubpassDraw && bEnableSubpassObserve){
		dependencies_mainscene.resize(3);

		//0->1
        dependencies_mainscene[0].srcSubpass = 0;//subpass0 is the src subpass, write in color image, output depth image
		dependencies_mainscene[0].dstSubpass = 1;//subpass1 is the dst subpass, input depth image, output color image(fragment shader convert depth image to color image)
		//
		dependencies_mainscene[0].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this is to make sure subpass0 finishes depth/color, then begin subpass1
		dependencies_mainscene[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //subpass0 need write depth image, subpass1 cant not access depth image before subpass 0 finish it
		dependencies_mainscene[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //this is to say subpass1's fragment shader can read depth image that subpass1 wrote(as input attachment)
		dependencies_mainscene[0].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; //subpass1 need to read input attachment(that subpass0 generate)

		dependencies_mainscene[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; //sync region instead of all framebuffer, to improve performance

		//1->2
		dependencies_mainscene[1].srcSubpass = 1;
		dependencies_mainscene[1].dstSubpass = 2;
		//
		dependencies_mainscene[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies_mainscene[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies_mainscene[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies_mainscene[1].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

		dependencies_mainscene[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		//0->2
		dependencies_mainscene[2].srcSubpass = 0;
		dependencies_mainscene[2].dstSubpass = 2;

		dependencies_mainscene[2].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies_mainscene[2].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies_mainscene[2].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies_mainscene[2].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

		dependencies_mainscene[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; 	
	}
}

void CRenderProcess::createRenderPass_mainscene(){ 
	//std::cout<<"Begin create renderpass"<<std::endl;
	VkResult result = VK_SUCCESS;

	//attachment descriptions in renderpass must match the same order as in framebuffer:
	//#1: a color attachment with sampler number = 1
	//#2: if depth test is enabled, a depth attachment with sampler number = n
	//#3: if msaa is enabled, a color attachment with sampler number = n
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	if(iAttachmentDepthLight >= 0) attachmentDescriptions.push_back(attachment_description_light_depth_mainscene);
	if(iAttachmentDepthCamera >= 0) attachmentDescriptions.push_back(attachment_description_depth_mainscene);
	if(iAttachmentColorResovle >= 0) attachmentDescriptions.push_back(attachment_description_color_resolve_mainscene);
	if(iAttachmentColorPresent >= 0) attachmentDescriptions.push_back(attachment_description_color_present_mainscene);

	//std::cout<<"Begin prepare renderpass info"<<std::endl;
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data(); //1
	renderPassInfo.subpassCount = subpasses_mainscene.size();//1;
	renderPassInfo.pSubpasses = subpasses_mainscene.data();//&subpass;//2
	renderPassInfo.dependencyCount = dependencies_mainscene.size();
	renderPassInfo.pDependencies = dependencies_mainscene.data(); //&dependency;//3
	//std::cout<<"Done prepare renderpass info: "<< subpasses.size()<<", "<<attachmentDescriptions.size()<<std::endl;

	result = vkCreateRenderPass(CContext::GetHandle().GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass_mainscene);
	//std::cout<<"Done create renderpass"<<std::endl;
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");	 


}

void CRenderProcess::create_attachment_description_light_depth_mainscene(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentLightDepth = true;

	//added for model
	attachment_description_light_depth_mainscene.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachment_description_light_depth_mainscene.samples = msaaSamples; //VK_SAMPLE_COUNT_1_BIT
	//attachment_description_light_depth.samples = VK_SAMPLE_COUNT_1_BIT; //hardware depthbias for shadowmap
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachment_description_light_depth_mainscene.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_light_depth_mainscene.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_light_depth_mainscene.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_light_depth_mainscene.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_light_depth_mainscene.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	attachment_description_light_depth_mainscene.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //will automatically change at the end of renderpass

}

void CRenderProcess::create_attachment_description_light_depth_shadowmap(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentLightDepth = true;

	//added for model
	attachment_description_light_depth_shadowmap.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachment_description_light_depth_shadowmap.samples = msaaSamples; //VK_SAMPLE_COUNT_1_BIT
	//attachment_description_light_depth.samples = VK_SAMPLE_COUNT_1_BIT; //hardware depthbias for shadowmap
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachment_description_light_depth_shadowmap.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_light_depth_shadowmap.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_light_depth_shadowmap.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_light_depth_shadowmap.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_light_depth_shadowmap.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachment_description_light_depth_shadowmap.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //will automatically change at the end of renderpass

}

void CRenderProcess::create_attachment_description_color_present_mainscene(VkFormat swapChainImageFormat){  
	//bUseAttachmentColorPresent = true;

	attachment_description_color_present_mainscene.format = swapChainImageFormat;
	attachment_description_color_present_mainscene.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description_color_present_mainscene.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_color_present_mainscene.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description_color_present_mainscene.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_color_present_mainscene.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_color_present_mainscene.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_color_present_mainscene.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void CRenderProcess::create_attachment_description_camera_depth_mainscene(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentDepth = true;

	//added for model
	attachment_description_depth_mainscene.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachment_description_depth_mainscene.samples = msaaSamples;
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachment_description_depth_mainscene.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//VK_ATTACHMENT_LOAD_OP_LOAD;//VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_depth_mainscene.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_depth_mainscene.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_depth_mainscene.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_depth_mainscene.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_depth_mainscene.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

}

void CRenderProcess::create_attachment_description_color_resolve_mainscene(VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples, VkImageLayout imageLayout){  
	//Concept of attachment in Vulkan is like render target in OpenGL
	//Subpass is a procedure to write/read attachments (a render process can has many subpasses, aka a render pass)
	//Subpass is designed for mobile TBDR architecture
	//At the beginning of subpass, attachment is loaded; at the end of attachment, attachment is stored
	//bUseAttachmentColorMultisample = true;

	attachment_description_color_resolve_mainscene.format = swapChainImageFormat;
	//std::cout<<"addColorAttachment::colorAttachment.format = "<<colorAttachment.format<<std::endl;
	attachment_description_color_resolve_mainscene.samples = msaaSamples;
	//std::cout<<"attachment_description_color_multisample.samples = "<<attachment_description_color_multisample.samples<<std::endl;
	attachment_description_color_resolve_mainscene.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_color_resolve_mainscene.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description_color_resolve_mainscene.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_color_resolve_mainscene.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_color_resolve_mainscene.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_color_resolve_mainscene.finalLayout = imageLayout;

	m_msaaSamples_renderProcess = msaaSamples;
	//std::cout<<"Color Attachment added. "<<std::endl;
}


void CRenderProcess::addColorBlendAttachment(VkBlendOp colorBlendOp, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, 
											 VkBlendOp alphaBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor){
	bUseColorBlendAttachment = true;

	//Blend Algorithm
	//oldColor: the color already in framebuffer
	//newColor: the color output from fragment shader
	//if blendEnable:
	//  finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb)
	//  finalColor.a   = (srcAlphaBlendFactor * newColor.a  ) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a  )
	//else:
	//  finalColor = newColor

	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorBlendAttachment.blendEnable = VK_TRUE;

	colorBlendAttachment.colorBlendOp = colorBlendOp;
	colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
	colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
	colorBlendAttachment.alphaBlendOp = alphaBlendOp;
	colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
	colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
}

void CRenderProcess::createComputePipelineLayout(VkDescriptorSetLayout &descriptorSetLayout){
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS) 
		throw std::runtime_error("failed to create compute pipeline layout!");
}
void CRenderProcess::createComputePipeline(VkShaderModule &computeShaderModule){
	bCreateComputePipeline = true;
	
	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
	computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeShaderStageInfo.module = computeShaderModule;
	computeShaderStageInfo.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = computePipelineLayout;
	pipelineInfo.stage = computeShaderStageInfo;

	if (vkCreateComputePipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}
}

void CRenderProcess::createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id){
	VkPushConstantRange dummyPushConstantRange;
	createGraphicsPipelineLayout(descriptorSetLayouts, dummyPushConstantRange, false, graphicsPipelineLayout_id);
}
void CRenderProcess::createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id){
	VkResult result = VK_SUCCESS;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// if (pt == PIPELINE_COMPUTE) {
	// 	pipelineLayoutInfo.setLayoutCount = 0;
	// 	pipelineLayoutInfo.pSetLayouts = nullptr;
	// }else {
	//std::cout<<"DEBUG: create graphics pipeline layout: descriptorSetLayouts.size()="<<descriptorSetLayouts.size()<<std::endl;
	pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();//  descriptorSetLayout;//todo: LAYOUT
	//}

	if(bUsePushConstant){
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	}

	//Create Graphics Pipeline Layout
	VkPipelineLayout newlayout;
	graphicsPipelineLayouts.push_back(newlayout);
	//std::cout<<"before vkCreatePipelineLayout()"<<std::endl;
	result = vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &graphicsPipelineLayouts[graphicsPipelineLayout_id]);
	//std::cout<<"after vkCreatePipelineLayout()"<<std::endl;
	//result = vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout);
	
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout!");
	//REPORT("vkCreatePipelineLayout");
}

void CRenderProcess::Cleanup(){
	if(renderPass_shadowmap != VK_NULL_HANDLE)
		vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderPass_shadowmap, nullptr);
	if(renderPass_mainscene != VK_NULL_HANDLE)
		vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderPass_mainscene, nullptr);

	if(bCreateGraphicsPipeline){
		for(int i = 0; i < graphicsPipelines.size(); i++){
			vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), graphicsPipelines[i], nullptr);
			vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), graphicsPipelineLayouts[i], nullptr);
		}
	}

	if(bCreateComputePipeline){
		vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), computePipeline, nullptr);
    	vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), computePipelineLayout, nullptr);
	}
	
}