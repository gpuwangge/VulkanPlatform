#include "../include/renderProcess.h"

CRenderProcess::CRenderProcess(){
    //debugger = new CDebugger("../logs/renderProcess.log");
    
    bUseColorAttachment = false;
    bUseDepthAttachment = false;
    bUseColorAttachmentResolve = false;

    m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    m_swapChainImageFormat = VK_FORMAT_UNDEFINED;

}
CRenderProcess::~CRenderProcess(){
	//if (!debugger) delete debugger;
}




void CRenderProcess::createSubpass(){ 
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
		colorAttachmentResolveRef.attachment = attachmentCount++;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pResolveAttachments = &colorAttachmentResolveRef; //added for MSAA
	}
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
}

void CRenderProcess::createDependency(VkPipelineStageFlags srcPipelineStageFlag, VkPipelineStageFlags dstPipelineStageFlag){  
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = srcPipelineStageFlag;
	dependency.srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	dependency.dstStageMask = dstPipelineStageFlag;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (bUseDepthAttachment) 
		dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
}

void CRenderProcess::createRenderPass(){ 
	//HERE_I_AM("wxjCreateRenderPass");

	VkResult result = VK_SUCCESS;

	std::vector<VkAttachmentDescription> attachments;
	if(bUseColorAttachment) attachments.push_back(colorAttachment);
	if(bUseDepthAttachment) attachments.push_back(depthAttachment);
	if(bUseColorAttachmentResolve) attachments.push_back(colorAttachmentResolve);

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data(); //1
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;//2
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;//3

	result = vkCreateRenderPass(CContext::GetHandle().GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");
	//REPORT("vkCreateRenderPass");
		 
}


void CRenderProcess::addColorAttachment(VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples, VkImageLayout imageLayout){  
    //Concept of attachment in Vulkan is like render target in OpenGL
	//Subpass is a procedure to write/read attachments (a render process can has many subpasses, aka a render pass)
	//Subpass is designed for mobile TBDR architecture
	//At the beginning of subpass, attachment is loaded; at the end of attachment, attachment is stored
	bUseColorAttachment = true;

    m_msaaSamples = msaaSamples;
    m_swapChainImageFormat = swapChainImageFormat;

	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = m_msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = imageLayout;
}
void CRenderProcess::addDepthAttachment(){  
	bUseDepthAttachment = true;

	//added for model
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = m_msaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
}
void CRenderProcess::addColorAttachmentResolve(){  
	bUseColorAttachmentResolve = true;

	//added for MSAA
	colorAttachmentResolve.format = m_swapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}


VkFormat CRenderProcess::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(CContext::GetHandle().GetPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat CRenderProcess::findDepthFormat() {
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
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

void CRenderProcess::createGraphicsPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout){
	VkPushConstantRange dummyPushConstantRange;
	createGraphicsPipelineLayout(descriptorSetLayout, dummyPushConstantRange, false);
}
void CRenderProcess::createGraphicsPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout, VkPushConstantRange &pushConstantRange, bool bUsePushConstant){
	VkResult result = VK_SUCCESS;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// if (pt == PIPELINE_COMPUTE) {
	// 	pipelineLayoutInfo.setLayoutCount = 0;
	// 	pipelineLayoutInfo.pSetLayouts = nullptr;
	// }else {
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;//  descriptorSetLayout;//todo: LAYOUT
	//}

	if(bUsePushConstant){
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	}

	//Create Graphics Pipeline Layout
	result = vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout!");
	//REPORT("vkCreatePipelineLayout");
}

void CRenderProcess::Cleanup(){
	if(renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderPass, nullptr);

	if(bCreateGraphicsPipeline){
	 	vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), graphicsPipeline, nullptr);
     	vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), graphicsPipelineLayout, nullptr);
	}

	if(bCreateComputePipeline){
		vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), computePipeline, nullptr);
    	vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), computePipelineLayout, nullptr);
	}
	
}