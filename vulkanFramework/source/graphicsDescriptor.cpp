#include "../include/graphicsDescriptor.h"

//Declare static variables here:
/************
* Pool
************/
int CGraphicsDescriptorManager::graphicsUniformTypes;
VkDescriptorPool CGraphicsDescriptorManager::graphicsDescriptorPool;
std::vector<VkDescriptorPoolSize> CGraphicsDescriptorManager::graphicsDescriptorPoolSizes;
unsigned int CGraphicsDescriptorManager::samplerSize;
void CGraphicsDescriptorManager::createDescriptorPool(unsigned int object_count){
    //Descriptor Step 1/3
    //objectCount = object_count;

	graphicsDescriptorPoolSizes.resize(getPoolSize());
	int counter = 0;
    //std::cout<<"createDescriptorPool::textureSamplers.size() = " << textureSamplers.size()<<std::endl;
    std::cout<<"Graphics Pool size = " << getPoolSize()<<std::endl;

    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM){
        //std::cout<<": Custom Buffer";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
        //std::cout<<": Lighting Buffer";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
	
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP || graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
        //std::cout<<": MVP";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	 	graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGE_SAMPLER){
        //std::cout<<": Sampler("<<samplerCount<<")";
        for(int i = 0; i < samplerSize; i++){
            graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	 	    graphicsDescriptorPoolSizes[counter].descriptorCount = object_count*static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///each object has a sampler
		    counter++;
        }
    }
    

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(graphicsDescriptorPoolSizes.size());
    //std::cout<<"poolInfo.poolSizeCount = "<<poolInfo.poolSizeCount <<std::endl;
	poolInfo.pPoolSizes = graphicsDescriptorPoolSizes.data();
	poolInfo.maxSets = ((counter==0)?1:counter)*10*static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!TODO: currently support 10 sets?

	VkResult result = vkCreateDescriptorPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &graphicsDescriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
	//REPORT("vkCreateDescriptorPool");
}

/************
* Layout
************/
std::vector<VkDescriptorSetLayoutBinding> CGraphicsDescriptorManager::graphicsBindings;
VkDescriptorSetLayout CGraphicsDescriptorManager::descriptorSetLayout_general;
VkDescriptorSetLayout CGraphicsDescriptorManager::descriptorSetLayout_texture;
void CGraphicsDescriptorManager::createDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding){
    //Descriptor Step 2/3
    graphicsBindings.resize(getLayoutSize_General());
	int bindingCounter = 0;
    std::cout<<"Layout(Graphics General) size = " << graphicsBindings.size()<<std::endl;

	if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM){
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = customBinding->descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = customBinding->descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = customBinding->pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = customBinding->stageFlags;
		bindingCounter++;
	}
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = m_lightingUBO.GetBinding().descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = m_lightingUBO.GetBinding().descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = m_lightingUBO.GetBinding().pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = m_lightingUBO.GetBinding().stageFlags;
		bindingCounter++;
        //std::cout<<"created lighting bindings "<<std::endl;
	}
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP){
        VkDescriptorSetLayoutBinding binding = MVPUniformBufferObject::GetBinding();
        //std::cout<<"DEBUG: MVP Layout binding="<<counter<<std::endl;
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = binding.descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = binding.descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = binding.pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = binding.stageFlags;
		bindingCounter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = binding.descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = binding.descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = binding.pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = binding.stageFlags;
		bindingCounter++;
    }
    

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(graphicsBindings.size());
	layoutInfo.pBindings = graphicsBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout_general);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	//REPORT("vkCreateDescriptorSetLayout");
    //std::cout<<"created bindings DONE"<<std::endl;
}
void CGraphicsDescriptorManager::createDescriptorSetLayout_Texture(){
    //Descriptor Step 2/3
    graphicsBindings.resize(textureSamplers.size());//sampleCount?
    std::cout<<"Layout(Graphcis Sampler) size = "<<textureSamplers.size()<<std::endl;
	int bindingCounter = 0;

    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGE_SAMPLER){
        for(int i = 0; i < textureSamplers.size(); i++){
            graphicsBindings[bindingCounter].binding = bindingCounter;
            graphicsBindings[bindingCounter].descriptorCount = 1;
            graphicsBindings[bindingCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            graphicsBindings[bindingCounter].pImmutableSamplers = nullptr;
            graphicsBindings[bindingCounter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindingCounter++;
        }
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(graphicsBindings.size());
	layoutInfo.pBindings = graphicsBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout_texture);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
}

/************
* Set
************/
void CGraphicsDescriptorManager::createDescriptorSets_General(){
//Descriptor Step 3/3
    //HERE_I_AM("wxjCreateDescriptorSets");

    int descriptorSize = getSetSize_General();
    //std::cout<<"Set(Graphics, Non-sampler) size = "<<getSetSize()<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_general);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = graphicsDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_general.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets_general.data());
    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {///!!!
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        //VkDescriptorBufferInfo storageBufferInfoLastFrame{}; //for compute shader
        //VkDescriptorBufferInfo storageBufferInfoCurrentFrame{}; //for compute shader

        descriptorWrites.resize(descriptorSize);
        int counter = 0;

        VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM){
            customBufferInfo.buffer = customUniformBuffers[i].buffer;
            customBufferInfo.offset = 0;
            customBufferInfo.range = m_customUniformBufferSize;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &customBufferInfo;
            counter++;
        }

        VkDescriptorBufferInfo lightingBufferInfo{}; //for lighting uniform
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
            lightingBufferInfo.buffer = m_lightingUniformBuffers[i].buffer;
            lightingBufferInfo.offset = 0;
            lightingBufferInfo.range = m_lightingUniformBufferSize;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &lightingBufferInfo;
            counter++;
        }

        VkDescriptorBufferInfo mvpBufferInfo{}; //for mvp
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP){ //TODO: Getbinding
            mvpBufferInfo.buffer = mvpUniformBuffers[i].buffer;
            mvpBufferInfo.offset = 0;
            //sizeof(MVPUniformBufferObject) is 512, including 2 mvp matrices. We only use one at a time.
            //spec requires alighment to be multiple of 256 (1080 TI). Maybe change this later?
            mvpBufferInfo.range = 256;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &mvpBufferInfo;
            counter++;
        }
        VkDescriptorBufferInfo vpBufferInfo{}; //for vp
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
            vpBufferInfo.buffer = vpUniformBuffers[i].buffer;
            vpBufferInfo.offset = 0;
            vpBufferInfo.range = sizeof(VPUniformBufferObject);
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &vpBufferInfo;
            counter++;
        }

        //Step 4
        vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    }
}


/************
* 1 GRAPHCIS_UNIFORMBUFFER_CUSTOM
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::customUniformBuffers; 
std::vector<void*> CGraphicsDescriptorManager::customUniformBuffersMapped;
VkDeviceSize CGraphicsDescriptorManager::m_customUniformBufferSize;
void CGraphicsDescriptorManager::addCustomUniformBuffer(VkDeviceSize customUniformBufferSize){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_CUSTOM;
    //std::cout<<"addCustomUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

	customUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	customUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	m_customUniformBufferSize = customUniformBufferSize;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = customUniformBuffers[i].init(m_customUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), customUniformBuffers[i].deviceMemory, 0, m_customUniformBufferSize, 0, &customUniformBuffersMapped[i]);
	}
}

/************
* 2 GRAPHCIS_UNIFORMBUFFER_LIGHTING
************/
LightingUniformBufferObject CGraphicsDescriptorManager::m_lightingUBO;
std::vector<CWxjBuffer> CGraphicsDescriptorManager::m_lightingUniformBuffers; 
std::vector<void*> CGraphicsDescriptorManager::m_lightingUniformBuffersMapped;
VkDeviceSize CGraphicsDescriptorManager::m_lightingUniformBufferSize;
void CGraphicsDescriptorManager::addLightingUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_LIGHTING;
    //std::cout<<"addCustomUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

	m_lightingUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_lightingUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	//m_customUniformBufferSize = customUniformBufferSize;
    m_lightingUniformBufferSize = sizeof(LightingUniformBufferObject);
    //std::cout<<"addLightingUniformBuffer::m_lightingUniformBufferSize = " << m_lightingUniformBufferSize<<std::endl;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = m_lightingUniformBuffers[i].init( m_lightingUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_lightingUniformBuffers[i].deviceMemory, 0,  m_lightingUniformBufferSize, 0, & m_lightingUniformBuffersMapped[i]);
	}
}


/************
* 3 GRAPHCIS_UNIFORMBUFFER_MVP
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::mvpUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
std::vector<void*> CGraphicsDescriptorManager::mvpUniformBuffersMapped;
MVPUniformBufferObject CGraphicsDescriptorManager::mvpUBO;
void CGraphicsDescriptorManager::addMVPUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_MVP;
    //std::cout<<"addMVPUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    mvpUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mvpUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = mvpUniformBuffers[i].init(sizeof(MVPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), mvpUniformBuffers[i].deviceMemory, 0, sizeof(MVPUniformBufferObject), 0, &mvpUniformBuffersMapped[i]);
    }
}


/************
* 4 GRAPHCIS_UNIFORMBUFFER_VP
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::vpUniformBuffers; 
std::vector<void*> CGraphicsDescriptorManager::vpUniformBuffersMapped;
VPUniformBufferObject CGraphicsDescriptorManager::vpUBO;
void CGraphicsDescriptorManager::addVPUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_VP;

    vpUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    vpUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = vpUniformBuffers[i].init(sizeof(VPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), vpUniformBuffers[i].deviceMemory, 0, sizeof(VPUniformBufferObject), 0, &vpUniformBuffersMapped[i]);
    }
}
bool CGraphicsDescriptorManager::CheckMVP(){ //to check if all objects associate this graphcis descriptor use MVP/VP or not. If return true, means it will use dynamic descriptor offset
    return ((graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP) 
         || (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP));
}


/************
* 5 GRAPHCIS_COMBINEDIMAGE_SAMPLER
************/
std::vector<VkSampler> CGraphicsDescriptorManager::textureSamplers;
void CGraphicsDescriptorManager::addImageSamplerUniformBuffer(std::vector<int> mipLevels){
    graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGE_SAMPLER;//non-static content
    //std::cout<<"addImageSamplerUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    samplerSize = mipLevels.size();

    for(int i = 0; i < mipLevels.size(); i++){
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(CContext::GetHandle().GetPhysicalDevice(), &properties);

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

        if (mipLevels[i] > 1) {
            //std::cout<<"Sampler MipLevels = "<< mipLevels<<"(>1). Enable mipmap for all textures using this sampler"<<std::endl;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;// VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(mipLevels[i] / 2);
            samplerInfo.mipLodBias = 0.0f;
        }

        VkSampler sampler;
        VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &sampler);
        //textureSamplers[textureSamplerCount++]
        textureSamplers.push_back(sampler);
        //samplerCount++;
        //REPORT("vkCreateSampler");
    }
}

/************
* Helper Functions
************/
int CGraphicsDescriptorManager::getPoolSize(){
	int descriptorPoolSize = 0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING) ? 1:0;
	descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP || graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP) ? 1:0;
	descriptorPoolSize += graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGE_SAMPLER ? samplerSize:0;

	return descriptorPoolSize;
}
int CGraphicsDescriptorManager::getLayoutSize_General(){
	int descriptorPoolSize = 0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING) ? 1:0;
	descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP || graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP) ? 1:0;

	return descriptorPoolSize;
}
int CGraphicsDescriptorManager::getSetSize_General(){
    return getLayoutSize_General();
}
void CGraphicsDescriptorManager::DestroyAndFree(){
    for(int i = 0; i < textureSamplers.size(); i++)
        vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureSamplers[i], nullptr);
    
    for (size_t i = 0; i < customUniformBuffers.size(); i++) 
        customUniformBuffers[i].DestroyAndFree();
    
    for (size_t i = 0; i < mvpUniformBuffers.size(); i++) 
        mvpUniformBuffers[i].DestroyAndFree();
    
    for (size_t i = 0; i < vpUniformBuffers.size(); i++) 
        vpUniformBuffers[i].DestroyAndFree();
    
    for (size_t i = 0; i < m_lightingUniformBuffers.size(); i++) 
        m_lightingUniformBuffers[i].DestroyAndFree();
    
    //no need to destroy descriptorSets, because they are from descriptorPool
    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), graphicsDescriptorPool, nullptr);//to be move to base class
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout_general, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout_texture, nullptr);
}
