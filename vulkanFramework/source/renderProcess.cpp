#include "../include/renderProcess.h"

void CRenderProcess::createSubpass(int attachment_id_to_observe){ 
	//uint32_t attachmentCount = 0;

	//clear values for each attachment. The array is indexed by attachment number
	//Only elements corresponding to cleared attachments are used. Other elements of pClearValues are ignored.
	//each attachment has its clearvalues

	//the subpass attachment ref must match the order in attachment description order
	//note that fragment shader will write color to buffer pointed by pColorAttachments
	//fragment shader output is not always single samplered, so if msaa is enabled, shader output can't be used to present; in this case, use pResolveAttachment for present

	//First: create attachment reference according to attachment info
	//attachment# must match framebuffer order
	/*
	if(bUseAttachmentDepth){//added for model
		clearValues.push_back({1.0f, 0}); //The range of depths in the depth buffer is 0.0 to 1.0 in Vulkan, where 1.0 lies at the far view plane and 0.0 at the near view plane.
		attachment_reference_depth.attachment = attachmentCount++; 
		attachment_reference_depth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	if(bUseAttachmentColorMultisample){ //added for MSAA
		clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f}); 
		attachment_reference_color_multisample.attachment = attachmentCount++; 
		attachment_reference_color_multisample.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if(bUseAttachmentColorPresent){
		clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f}); //color attachment: the background color is set to black (0,0,0,1)
		attachment_reference_color_present.attachment = attachmentCount++; 
		attachment_reference_color_present.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	//Second: create subpasses
	//subpass dont need to use all attachments
	//set subpass# in pipeline(s)
	VkSubpassDescription subpass0{};
	subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass0.colorAttachmentCount = 1;
	if(bUseAttachmentColorPresent){
		if(bUseAttachmentColorMultisample) subpass0.pResolveAttachments = &attachment_reference_color_present;
		else subpass0.pColorAttachments = &attachment_reference_color_present; //fragment shader output here
	}
	if(bUseAttachmentDepth) subpass0.pDepthStencilAttachment = &attachment_reference_depth; 
	if(bUseAttachmentColorMultisample) subpass0.pColorAttachments = &attachment_reference_color_multisample; //fragment shader output here
	
	VkSubpassDescription subpass1{};
	subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass1.colorAttachmentCount = 1;
	if(bUseAttachmentColorPresent){
		if(bUseAttachmentColorMultisample) subpass1.pResolveAttachments = &attachment_reference_color_present;
		else subpass1.pColorAttachments = &attachment_reference_color_present; //fragment shader output here
	}
	if(bUseAttachmentDepth) subpass1.pDepthStencilAttachment = &attachment_reference_depth; 
	if(bUseAttachmentColorMultisample) subpass1.pColorAttachments = &attachment_reference_color_multisample; //fragment shader output here

	if(bDepth) subpasses.push_back(subpass0);
	if(bNormal) subpasses.push_back(subpass1);

	*/
	
	std::cout<<"Begin create subpasses"<<std::endl;

	switch(m_renderFeature){
		case RenderFeatures::PRESENT:
			clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
		break;
		case RenderFeatures::PRESENT_DEPTH: 
			clearValues.push_back({1.0f, 0}); 
			clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
		break;
		case RenderFeatures::PRESENT_DEPTH_MSAA:
		if(!bEnableSubpassShadowmap){
			clearValues.push_back({1.0f, 0}); 
			clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
			clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
		}else{
			clearValues.push_back({1.0f, 0}); 
			clearValues.push_back({1.0f, 0}); 
			clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
			clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
		}	
		break;
	}
	

	//attachmentDescriptions(.attachment) order: 
	//if PRESENT: 0-present_color
	//if PRESENT_DEPTH: 0-depth; 1-presentcolor
	//if PRESENT_DEPTH_MSAA: 0-depth; 1-msaacolor; 2-presentcolor
	//if ?: 0-lightdepth; 1-depth; 2-msaacolor; 3-presentcolor

	if(bEnableSubpassShadowmap){
		VkSubpassDescription subpass{};//to generate light depth for shadowmap
		switch(m_renderFeature){
			case RenderFeatures::PRESENT:
			break;
			case RenderFeatures::PRESENT_DEPTH:
			break;
			case RenderFeatures::PRESENT_DEPTH_MSAA:
				attachmentRef_light_depth_shadowmap.attachment = iAttachmentDepthLight; 
				attachmentRef_light_depth_shadowmap.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format
				subpass.pDepthStencilAttachment = &attachmentRef_light_depth_shadowmap;
				//subpass.pDepthStencilAttachment = &attachmentRef_depth_draw;
			break;
		}
		subpass.colorAttachmentCount = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
		subpasses.push_back(subpass);
	}




	if(bEnableSubpassDraw){
		VkSubpassDescription subpass{};
		switch(m_renderFeature){
			case RenderFeatures::PRESENT: 
				attachmentRef_color_draw.attachment = iAttachmentColorPresent; 
				attachmentRef_color_draw.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pColorAttachments = &attachmentRef_color_draw;
			break;
			case RenderFeatures::PRESENT_DEPTH: //0-depth; 1-presentcolor(fragment output to this)
				attachmentRef_depth_draw.attachment = iAttachmentDepthCamera; 
				attachmentRef_depth_draw.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format
				subpass.pDepthStencilAttachment = &attachmentRef_depth_draw;
				
				attachmentRef_color_draw.attachment = iAttachmentColorPresent; 
				attachmentRef_color_draw.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pColorAttachments = &attachmentRef_color_draw;
			break;
			case RenderFeatures::PRESENT_DEPTH_MSAA://0-depth; 1-msaacolor(fragment output to this); 2-presentcolor
				attachmentRef_depth_draw.attachment = iAttachmentDepthCamera; 
				attachmentRef_depth_draw.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format
				subpass.pDepthStencilAttachment = &attachmentRef_depth_draw;
				
				attachmentRef_color_multisample_draw.attachment = iAttachmentColorResovle; 
				attachmentRef_color_multisample_draw.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pColorAttachments = &attachmentRef_color_multisample_draw;

				attachmentRef_color_draw.attachment = iAttachmentColorPresent; 
				attachmentRef_color_draw.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pResolveAttachments = &attachmentRef_color_draw;
			break;
		}
		subpass.colorAttachmentCount = 1;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
		subpasses.push_back(subpass);
	}

	if(bEnableSubpassObserve){
		VkSubpassDescription subpass{};
		switch(m_renderFeature){
			case RenderFeatures::PRESENT: //0-present_color
				//cant not run oberserve subpass if it has only color attachment??
			break;
			case RenderFeatures::PRESENT_DEPTH://0-depth; 1-presentcolor(fragment output to this)
				attachmentRef_observe.attachment = iAttachmentDepthCamera;
				attachmentRef_observe.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; //depth image format for output
				subpass.pInputAttachments = &attachmentRef_observe;
				
				attachmentRef_color_observe.attachment = iAttachmentColorPresent; 
				attachmentRef_color_observe.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pColorAttachments = &attachmentRef_color_observe;
			break;
			case RenderFeatures::PRESENT_DEPTH_MSAA://0-depth; 1-msaacolor(fragment output to this); 2-presentcolor
				attachmentRef_observe.attachment = attachment_id_to_observe; //iAttachmentDepthCamera or iAttachmentDepthLight
				attachmentRef_observe.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; //depth image format for output
				subpass.pInputAttachments = &attachmentRef_observe;
				
				attachmentRef_color_multisample_observe.attachment = iAttachmentColorResovle; 
				attachmentRef_color_multisample_observe.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pColorAttachments = &attachmentRef_color_multisample_observe;

				attachmentRef_color_observe.attachment = iAttachmentColorPresent; 
				attachmentRef_color_observe.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				subpass.pResolveAttachments = &attachmentRef_color_observe;
			break;
		}
		subpass.colorAttachmentCount = 1;
		subpass.inputAttachmentCount = 1;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses.push_back(subpass);
	}

	std::cout<<"Done create subpasses"<<std::endl;
}

void CRenderProcess::createDependency(){  
	if(!bEnableSubpassShadowmap && bEnableSubpassDraw && !bEnableSubpassObserve){ //single subpass
		switch(m_renderFeature){
			case RenderFeatures::PRESENT:
			break;
			case RenderFeatures::PRESENT_DEPTH:
			break;
			case RenderFeatures::PRESENT_DEPTH_MSAA:
			break;
		}

		if (iAttachmentDepthCamera < 0) {
			VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = srcPipelineStageFlag;
			dependency.srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			dependency.dstStageMask = dstPipelineStageFlag;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}else{
			VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = srcPipelineStageFlag;
			dependency.srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			dependency.dstStageMask = dstPipelineStageFlag;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
	}

	if(!bEnableSubpassShadowmap && bEnableSubpassDraw && bEnableSubpassObserve){ //two subpasses
		switch(m_renderFeature){
			case RenderFeatures::PRESENT:
			break;
			case RenderFeatures::PRESENT_DEPTH:
			break;
			case RenderFeatures::PRESENT_DEPTH_MSAA:
			break;
		}

        dependency.srcSubpass = 0;//subpass0 is the src subpass, write in color image, output depth image
		dependency.dstSubpass = 1;//subpass1 is the dst subpass, input depth image, output color image(fragment shader convert depth image to color image)
		
		dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this is to make sure subpass0 finishes depth/color, then begin subpass1
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //subpass0 need write depth image, subpass1 cant not access depth image before subpass 0 finish it
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //this is to say subpass1's fragment shader can read depth image that subpass1 wrote(as input attachment)
		dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; //subpass1 need to read input attachment(that subpass0 generate)

		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; //sync region instead of all framebuffer, to improve performance
	}

	if(bEnableSubpassShadowmap && bEnableSubpassDraw && bEnableSubpassObserve){
		switch(m_renderFeature){
			case RenderFeatures::PRESENT:
			break;
			case RenderFeatures::PRESENT_DEPTH:
			break;
			case RenderFeatures::PRESENT_DEPTH_MSAA:
			break;
		}

        dependency.srcSubpass = 0;//subpass0 is the src subpass, write in color image, output depth image
		dependency.dstSubpass = 1;//subpass1 is the dst subpass, input depth image, output color image(fragment shader convert depth image to color image)
		
		dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this is to make sure subpass0 finishes depth/color, then begin subpass1
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //subpass0 need write depth image, subpass1 cant not access depth image before subpass 0 finish it
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //this is to say subpass1's fragment shader can read depth image that subpass1 wrote(as input attachment)
		dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; //subpass1 need to read input attachment(that subpass0 generate)

		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; //sync region instead of all framebuffer, to improve performance		
	}



	//SINGLE SUBPASS
	// dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	// dependency.dstSubpass = 0;
	// dependency.srcStageMask = srcPipelineStageFlag;
	// dependency.srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	// dependency.dstStageMask = dstPipelineStageFlag;
	// dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	// if (bUseAttachmentDepth) 
	// 	dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	//TEST TWO SUBPASSES

	//if enable depthtest
	//VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    //VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	

	//  dependency.srcStageMask = srcPipelineStageFlag;//VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	//  dependency.dstStageMask = dstPipelineStageFlag;//VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//  dependency.srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;//VK_ACCESS_SHADER_READ_BIT;

	

}

void CRenderProcess::createRenderPass(){ 
	//std::cout<<"Begin create renderpass"<<std::endl;
	VkResult result = VK_SUCCESS;

	//attachment descriptions in renderpass must match the same order as in framebuffer:
	//#1: a color attachment with sampler number = 1
	//#2: if depth test is enabled, a depth attachment with sampler number = n
	//#3: if msaa is enabled, a color attachment with sampler number = n
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	if(iAttachmentDepthLight >= 0) attachmentDescriptions.push_back(attachment_description_light_depth);
	if(iAttachmentDepthCamera >= 0) attachmentDescriptions.push_back(attachment_description_depth);
	if(iAttachmentColorResovle >= 0) attachmentDescriptions.push_back(attachment_description_color_multisample);
	if(iAttachmentColorPresent >= 0) attachmentDescriptions.push_back(attachment_description_color_present); 

	//std::cout<<"Begin prepare renderpass info"<<std::endl;
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data(); //1
	renderPassInfo.subpassCount = subpasses.size();//1;
	renderPassInfo.pSubpasses = subpasses.data();//&subpass;//2
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;//3
	//std::cout<<"Done prepare renderpass info: "<< subpasses.size()<<", "<<attachmentDescriptions.size()<<std::endl;

	result = vkCreateRenderPass(CContext::GetHandle().GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
	//std::cout<<"Done create renderpass"<<std::endl;
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");	 


}

void CRenderProcess::enableAttachmentDescriptionLightDepth(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentLightDepth = true;

	//added for model
	attachment_description_light_depth.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachment_description_light_depth.samples = msaaSamples;
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachment_description_light_depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_light_depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_light_depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_light_depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_light_depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_light_depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
}

void CRenderProcess::enableColorAttachmentDescriptionColorPresent(VkFormat swapChainImageFormat){  
	//bUseAttachmentColorPresent = true;

	attachment_description_color_present.format = swapChainImageFormat;
	attachment_description_color_present.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description_color_present.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_color_present.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description_color_present.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_color_present.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_color_present.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_color_present.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void CRenderProcess::enableAttachmentDescriptionDepth(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentDepth = true;

	//added for model
	attachment_description_depth.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachment_description_depth.samples = msaaSamples;
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachment_description_depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
}

void CRenderProcess::enableAttachmentDescriptionColorMultiSample(VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples, VkImageLayout imageLayout){  
	//Concept of attachment in Vulkan is like render target in OpenGL
	//Subpass is a procedure to write/read attachments (a render process can has many subpasses, aka a render pass)
	//Subpass is designed for mobile TBDR architecture
	//At the beginning of subpass, attachment is loaded; at the end of attachment, attachment is stored
	//bUseAttachmentColorMultisample = true;

	attachment_description_color_multisample.format = swapChainImageFormat;
	//std::cout<<"addColorAttachment::colorAttachment.format = "<<colorAttachment.format<<std::endl;
	attachment_description_color_multisample.samples = msaaSamples;
	//std::cout<<"attachment_description_color_multisample.samples = "<<attachment_description_color_multisample.samples<<std::endl;
	attachment_description_color_multisample.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_color_multisample.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description_color_multisample.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_color_multisample.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_color_multisample.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_color_multisample.finalLayout = imageLayout;

	m_msaaSamples = msaaSamples;
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
	if(renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderPass, nullptr);

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