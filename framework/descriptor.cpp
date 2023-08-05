#include "descriptor.h"
#include "application.h"

//Camera CApplication::mainCamera;

CDescriptor::CDescriptor(){
    debugger = new CDebugger("../logs/descriptorLog.txt");
	bUseCustomUniformBuffer = false;
    bUseMVP = false;
    bUseSampler = false;
	m_customUniformBufferSize = 0;
	mvpUBO.model = glm::mat4(1.0f);
    textureSampler = NULL;
}

CDescriptor::~CDescriptor(){}

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
        //VkResult result = InitDataBufferHelper(bufferSize, usage, &_uniformBuffers[i]);
        VkResult result = mvpUniformBuffers[i].init(sizeof(MVPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        REPORT("InitDataBufferHelper");

        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), mvpUniformBuffers[i].deviceMemory, 0, sizeof(MVPUniformBufferObject), 0, &mvpUniformBuffersMapped[i]);
    }

    //descriptorTypes.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	//shaderStageFlagBits.push_back(VK_SHADER_STAGE_VERTEX_BIT);
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
	REPORT("vkCreateSampler");

    //descriptorTypes.push_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    //shaderStageFlagBits.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
}


void CDescriptor::createDescriptorPool(VkDescriptorType type){
	HERE_I_AM("wxjCreateDescriptorPool");

	poolSizes.resize(getDescriptorSize());
	int counter = 0;

	if(bUseCustomUniformBuffer){
        poolSizes[counter].type = type;
	 	poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
	
    if(bUseMVP){
        poolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
    }

    if(bUseSampler){
        poolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	 	poolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
    }


	// for(int i = 0; i < descriptorTypes.size(); i++){
	// 	poolSizes[i].type = descriptorTypes[i];
	// 	poolSizes[i].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	// }

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
	VkResult result = vkCreateDescriptorPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
	REPORT("vkCreateDescriptorPool");
}

void CDescriptor::createDescriptorSetLayout(VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount, const VkSampler*  pImmutableSamplers){
	HERE_I_AM("wxjCreateDescriptorSetLayout");

	//VkDescriptorSetLayoutBinding uboLayoutBinding{};
	//std::vector<VkDescriptorSetLayoutBinding> bindings;
	//bindings.resize(descriptorTypes.size());
	// for(int i = 0; i < descriptorTypes.size(); i++){
	// 	bindings[i].binding = i;
	// 	bindings[i].descriptorCount = 1;
	// 	bindings[i].descriptorType = descriptorTypes[i];
	// 	bindings[i].pImmutableSamplers = nullptr;
	// 	bindings[i].stageFlags = shaderStageFlagBits[i];
	// }


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

    bindings.resize(getDescriptorSize());
	int counter = 0;

	if(bUseCustomUniformBuffer){
        bindings[counter].binding = counter;
		bindings[counter].descriptorCount = descriptorCount;
		bindings[counter].descriptorType = descriptorType;
		bindings[counter].pImmutableSamplers = pImmutableSamplers;
		bindings[counter].stageFlags = stageFlags;
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

    if(bUseSampler){
        bindings[counter].binding = counter;
        bindings[counter].descriptorCount = 1;
        bindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[counter].pImmutableSamplers = nullptr;
        bindings[counter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        counter++;
    }
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	//layoutInfo.bindingCount = 1;//TODO: test
	//auto binding = UniformBufferObject::GetBinding();//TODO: test
	//binding.binding = 0;//TODO: test
	//layoutInfo.pBindings = &binding;//TODO: test

	//Step 2
	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	REPORT("vkCreateDescriptorSetLayout");
}

void CDescriptor::createDescriptorSets(VkImageView *textureImageView){
        HERE_I_AM("wxjCreateDescriptorSets");

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
        REPORT("vkAllocateDescriptorSets");

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform
            VkDescriptorBufferInfo mvpBufferInfo{}; //for mvp
            VkDescriptorImageInfo imageInfo{}; //for texture sampler

            VkDescriptorBufferInfo storageBufferInfoLastFrame{}; //for compute shader
            VkDescriptorBufferInfo storageBufferInfoCurrentFrame{}; //for compute shader

            descriptorWrites.resize(descriptorSize);
            //for(int j = 0; j < descriptorTypes.size(); j++){
            int counter = 0;

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
            //}

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
            //vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrites[0], 0, nullptr);

        }
    }


void CDescriptor::updateMVPUniformBuffer(uint32_t currentFrame, float durationTime) {
    if(bUseMVP){
        // UniformBufferObject ubo{};
        // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // glm::mat4x4 m = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10.0f);
        // m[1][1] *= -1;
        // ubo.proj = m;
        // ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        //UniformBufferObject ubo{};
        mvpUBO.view = CApplication::mainCamera.matrices.view;
        mvpUBO.proj = CApplication::mainCamera.matrices.perspective;
        // if (pipelineType == PIPELINE_MIPMAP) {
                //ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // }
        // else {
            //ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //}

        memcpy(mvpUniformBuffersMapped[currentFrame], &mvpUBO, sizeof(mvpUBO));

        //CApplication::updateUniformBuffer(currentFrame, durationTime);
    }
}

int CDescriptor::getDescriptorSize(){
	int descriptorSize = 0;
	descriptorSize += bUseCustomUniformBuffer ? 1:0;
	descriptorSize += bUseMVP ? 1:0;
	descriptorSize += bUseSampler ? 1:0;
	return descriptorSize;
}

void CDescriptor::DestroyAndFree(){
    if(textureSampler) vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureSampler, nullptr);

    for (size_t i = 0; i < customUniformBuffers.size(); i++) {
        customUniformBuffers[i].DestroyAndFree();
    }

    for (size_t i = 0; i < mvpUniformBuffers.size(); i++) {
        mvpUniformBuffers[i].DestroyAndFree();
    }

    //no need to destroy descriptorSets, because they are from descriptorPool
    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout, nullptr);
    if (!debugger) delete debugger;
}

