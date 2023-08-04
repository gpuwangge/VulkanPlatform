#include "descriptor.h"

CDescriptor::CDescriptor(){
    debugger = new CDebugger("../logs/descriptorLog.txt");
    bUseMVP = false;
    bUseSampler = false;
    textureSampler = NULL;
}

CDescriptor::~CDescriptor(){

}

void CDescriptor::addMVPUniformBuffer(){
    bUseMVP = true;
	
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //VkResult result = InitDataBufferHelper(bufferSize, usage, &_uniformBuffers[i]);
        VkResult result = uniformBuffers[i].init(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        REPORT("InitDataBufferHelper");

        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), uniformBuffers[i].deviceMemory, 0, sizeof(UniformBufferObject), 0, &uniformBuffersMapped[i]);
    }

    descriptorTypes.push_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	shaderStageFlagBits.push_back(VK_SHADER_STAGE_VERTEX_BIT);
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

    descriptorTypes.push_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    shaderStageFlagBits.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
}


void CDescriptor::createDescriptorPool(){
	HERE_I_AM("wxjCreateDescriptorPool");

	std::vector<VkDescriptorPoolSize> poolSizes;
	//poolSizes.resize(descriptorTypes.size());
    if(bUseMVP){
        VkDescriptorPoolSize poolSize;
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes.push_back(poolSize);
    }
    if(bUseSampler){
        VkDescriptorPoolSize poolSize;
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	 	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes.push_back(poolSize);
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

void CDescriptor::createDescriptorSetLayout(){
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

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    if(bUseMVP){
        VkDescriptorSetLayoutBinding binding = UniformBufferObject::GetBinding();
        binding.binding = 0;//TODO: test
        bindings.push_back(binding);
    }
    if(bUseSampler){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 1;//TODO: test
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(binding);
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

void CDescriptor::createDescriptorSets(VkImageView &textureImageView){
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
	result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets.data());
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
		vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		//vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrites[0], 0, nullptr);

	}
}


void CDescriptor::DestroyAndFree(){
    if(textureSampler) vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureSampler, nullptr);

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        uniformBuffers[i].DestroyAndFree();
    }

    //no need to destroy descriptorSets, because they are from descriptorPool
    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout, nullptr);
    if (!debugger) delete debugger;
}

