#include "descriptor.h"

//Camera CApplication::mainCamera;

CDescriptor::CDescriptor(){
    //debugger = new CDebugger("../logs/descriptor.log");
	bUseCustomUniformBuffer = false;
    bUseMVP = false;
    bUseVP = false;
    bUseSampler = false;
    bUseComputeStorage = false;
	m_customUniformBufferSize = 0;
	mvpUBO.model = glm::mat4(1.0f);
    textureSampler = NULL;
}

CDescriptor::~CDescriptor(){
    //if (!debugger) delete debugger;
}

void CDescriptor::addCustomUniformBuffer(VkDeviceSize customUniformBufferSize){
	bUseCustomUniformBuffer = true;

	customUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	customUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	m_customUniformBufferSize = customUniformBufferSize;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = customUniformBuffers[i].init(m_customUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), customUniformBuffers[i].deviceMemory, 0, m_customUniformBufferSize, 0, &customUniformBuffersMapped[i]);
	}
}

void CDescriptor::addMVPUniformBuffer(){
    bUseMVP = true;
	
    mvpUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mvpUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = mvpUniformBuffers[i].init(sizeof(MVPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        //REPORT("InitDataBufferHelper");

        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), mvpUniformBuffers[i].deviceMemory, 0, sizeof(MVPUniformBufferObject), 0, &mvpUniformBuffersMapped[i]);
    }
}

void CDescriptor::addVPUniformBuffer(){
    bUseVP = true;
	
    vpUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    vpUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = vpUniformBuffers[i].init(sizeof(VPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        //REPORT("InitDataBufferHelper");

        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), vpUniformBuffers[i].deviceMemory, 0, sizeof(VPUniformBufferObject), 0, &vpUniformBuffersMapped[i]);
    }
}

void CDescriptor::addImageSamplerUniformBuffer(uint32_t mipLevels){
    bUseSampler = true;

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
	
	if (mipLevels > 1) {
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;// VK_SAMPLER_MIPMAP_MODE_NEAREST;
	 	samplerInfo.minLod = 0.0f;
	 	samplerInfo.maxLod = static_cast<float>(mipLevels / 2);
	 	samplerInfo.mipLodBias = 0.0f;
	}

	VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &textureSampler);
	//REPORT("vkCreateSampler");
}

void CDescriptor::addComputeStorageUniformBuffer(){
    bUseComputeStorage = true;
    
}

void CDescriptor::createDescriptorPool(VkDescriptorType type){
	//HERE_I_AM("CreateDescriptorPool");

	poolSizes.resize(getDescriptorSize());
	int counter = 0;

	if(bUseCustomUniformBuffer){
        poolSizes[counter].type = type;
	 	poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
	
    if(bUseMVP || bUseVP){
        poolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
    }

    if(bUseSampler){
        poolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	 	poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
    }

    if(bUseComputeStorage){
        poolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	    poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); 
        counter++;
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

	VkResult result = vkCreateDescriptorPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
	//REPORT("vkCreateDescriptorPool");
}

void CDescriptor::createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding){
    //VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount, const VkSampler*  pImmutableSamplers){
	//HERE_I_AM("CreateDescriptorSetLayout");

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

    bindings.resize(getDescriptorSize());
	int counter = 0;

	if(bUseCustomUniformBuffer){
        bindings[counter].binding = counter;
		bindings[counter].descriptorCount = customBinding->descriptorCount;
		bindings[counter].descriptorType = customBinding->descriptorType;
		bindings[counter].pImmutableSamplers = customBinding->pImmutableSamplers;
		bindings[counter].stageFlags = customBinding->stageFlags;
		counter++;
	}

    if(bUseMVP){
        VkDescriptorSetLayoutBinding binding = MVPUniformBufferObject::GetBinding();
        bindings[counter].binding = counter;
		bindings[counter].descriptorCount = binding.descriptorCount;
		bindings[counter].descriptorType = binding.descriptorType;
		bindings[counter].pImmutableSamplers = binding.pImmutableSamplers;
		bindings[counter].stageFlags = binding.stageFlags;
		counter++;
    }

    if(bUseVP){
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        bindings[counter].binding = counter;
		bindings[counter].descriptorCount = binding.descriptorCount;
		bindings[counter].descriptorType = binding.descriptorType;
		bindings[counter].pImmutableSamplers = binding.pImmutableSamplers;
		bindings[counter].stageFlags = binding.stageFlags;
		counter++;
    }

    if(bUseSampler){
        bindings[counter].binding = counter;
        bindings[counter].descriptorCount = 1;
        bindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[counter].pImmutableSamplers = nullptr;
        bindings[counter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        counter++;
    }

    if(bUseComputeStorage){
        bindings[counter].binding = counter;
        bindings[counter].descriptorCount = 1;
        bindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[counter].pImmutableSamplers = nullptr;
        bindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;

        // bindings[counter].binding = counter;
        // bindings[counter].descriptorCount = 1;
        // bindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        // bindings[counter].pImmutableSamplers = nullptr;
        // bindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;       
        // counter++;
    }


	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	//REPORT("vkCreateDescriptorSetLayout");
}

void CDescriptor::createDescriptorSets(VkImageView *textureImageView){
        //HERE_I_AM("wxjCreateDescriptorSets");

        int descriptorSize = getDescriptorSize();

        VkResult result = VK_SUCCESS;

        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        //Step 3
        result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");
        //REPORT("vkAllocateDescriptorSets");

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            //VkDescriptorBufferInfo storageBufferInfoLastFrame{}; //for compute shader
            //VkDescriptorBufferInfo storageBufferInfoCurrentFrame{}; //for compute shader

            descriptorWrites.resize(descriptorSize);
            int counter = 0;

            VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform
            if(bUseCustomUniformBuffer){
                customBufferInfo.buffer = customUniformBuffers[i].buffer;
                customBufferInfo.offset = 0;
                customBufferInfo.range = m_customUniformBufferSize;
                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pBufferInfo = &customBufferInfo;
                counter++;
            }

            VkDescriptorBufferInfo mvpBufferInfo{}; //for mvp
            if(bUseMVP){
                mvpBufferInfo.buffer = mvpUniformBuffers[i].buffer;
                mvpBufferInfo.offset = 0;
                mvpBufferInfo.range = sizeof(MVPUniformBufferObject);
                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pBufferInfo = &mvpBufferInfo;
                counter++;
            }

            VkDescriptorBufferInfo vpBufferInfo{}; //for vp
            if(bUseVP){
                vpBufferInfo.buffer = vpUniformBuffers[i].buffer;
                vpBufferInfo.offset = 0;
                vpBufferInfo.range = sizeof(VPUniformBufferObject);
                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pBufferInfo = &vpBufferInfo;
                counter++;
            }

            VkDescriptorImageInfo imageInfo{}; //for texture sampler
            if(bUseSampler){
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = *textureImageView;
                imageInfo.sampler = textureSampler;
                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pImageInfo = &imageInfo;
                counter++;
            }

            VkDescriptorBufferInfo storageBufferInfo{};
            if(bUseComputeStorage){
                storageBufferInfo.buffer = shaderStorageBuffers_compute[i].buffer;//shaderStorageBuffers_compute[i].buffer;
                storageBufferInfo.offset = 0;
                storageBufferInfo.range =sizeof(uint32_t) * 4;//sizeof(Particle) * PARTICLE_COUNT;

                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pBufferInfo = &storageBufferInfo;
                counter++;
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
            vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        }
    }


void CDescriptor::updateMVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera) {
    if(bUseMVP){
        // UniformBufferObject ubo{};
        // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // glm::mat4x4 m = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10.0f);
        // m[1][1] *= -1;
        // ubo.proj = m;
        // ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        //UniformBufferObject ubo{};
        mvpUBO.view = mainCamera.matrices.view;
        mvpUBO.proj = mainCamera.matrices.perspective;

        memcpy(mvpUniformBuffersMapped[currentFrame], &mvpUBO, sizeof(mvpUBO));

        //CApplication::updateUniformBuffer(currentFrame, durationTime);
    }
}

void CDescriptor::updateVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera) {
    if(bUseVP){
        //UniformBufferObject ubo{};
        vpUBO.view = mainCamera.matrices.view;
        vpUBO.proj = mainCamera.matrices.perspective;

        memcpy(vpUniformBuffersMapped[currentFrame], &vpUBO, sizeof(vpUBO));

        //CApplication::updateUniformBuffer(currentFrame, durationTime);
    }
}

int CDescriptor::getDescriptorSize(){
	int descriptorSize = 0;
	descriptorSize += bUseCustomUniformBuffer ? 1:0;
	descriptorSize += (bUseMVP||bUseVP) ? 1:0;
	descriptorSize += bUseSampler ? 1:0;
    descriptorSize += bUseComputeStorage ? 1:0;
	return descriptorSize;
}

void CDescriptor::createShaderStorageBuffers(){
    VkDeviceSize bufferSize = sizeof(uint32_t) * 4;
    shaderStorageBuffers_compute.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		//VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &shaderStorageBuffers_compute[i]);// Create a staging buffer used to upload data to the gpu
		//FillDataBufferHelper(shaderStorageBuffers_compute[i], (void *)(particles.data()));// Copy initial particle data to all storage buffers
        //shaderStorageBuffers_compute[i].init(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        shaderStorageBuffers_compute[i].init(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT );
	}
}

void CDescriptor::DestroyAndFree(){
    if(textureSampler) vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureSampler, nullptr);

    for (size_t i = 0; i < customUniformBuffers.size(); i++) {
        customUniformBuffers[i].DestroyAndFree();
    }

    for (size_t i = 0; i < mvpUniformBuffers.size(); i++) {
        mvpUniformBuffers[i].DestroyAndFree();
    }

    for (size_t i = 0; i < vpUniformBuffers.size(); i++) {
        vpUniformBuffers[i].DestroyAndFree();
    }

    for (size_t i = 0; i < shaderStorageBuffers_compute.size(); i++) {
        shaderStorageBuffers_compute[i].DestroyAndFree();
    }

    //no need to destroy descriptorSets, because they are from descriptorPool
    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout, nullptr);
}

