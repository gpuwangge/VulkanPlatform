#include "..\\framework\\vulkanBase.h"

class CSimpleTriangle: public CVulkanBase{
public:
    CSimpleTriangle(){
		/************
		buffersize: 8(numbers each vertex)*4(float)*4(vertex size)=128（byte）
		************/
		vertices3D = {
		 	{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		 	{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
		 	{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
		 	{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
		 };

		/************
		buffer size: 6*4=24（byte）
		************/
		indices3D = { 0, 1, 2, 2, 3, 0};

		vertexShaderPath = "../shaders/basic/vert.spv";
    	fragmentShaderPath = "../shaders/basic/frag.spv";

		/*****
		 * Other things to prepare
		 * Renderpass: colorAttachment, depthAttachment(for model), colorAttachmentResolve(for MSAA)
		 * Framebuffer: swapChainImageViews[i], depthImageView(for model), colorImageView_msaa(for MSAA)
		 * Descriptor: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER (for MVP),VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER (for texture), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER (for compute shader)
		 * ****/
    }

    ~CSimpleTriangle(){

    }

	void updateUniformBuffer(uint32_t currentFrame, float durationTime) {

		CVulkanBase::updateUniformBuffer(currentFrame, durationTime);
	}

	void update(){
		//printf("triangle update...\n");

		CVulkanBase::update();
	}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		CVulkanBase::recordCommandBuffer(commandBuffer, imageIndex);
	}

	void drawFrame(){
		//printf("triangle drawFrame...\n");

		CVulkanBase::drawFrame();
	}

	void initVulkan(){
		printf("simpleTriangle init\n");
		CVulkanBase::initVulkan();
	}

	void CreateRenderPass() {
		wxjCreateColorAttachment();
		//wxjCreatDepthAttachment();
		//wxjCreatColorAttachmentResolve();
		wxjCreateSubpass();
		wxjCreateDependency();
		wxjCreateRenderPass();
	}

	void CreateFramebuffers() {
		wxjCreateFramebuffers();

		// HERE_I_AM("Init11CreateFramebuffers");

		// VkResult result = VK_SUCCESS;

		// swapChainFramebuffers.resize(swapChainImageViews.size());

		// for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		// 	std::vector<VkImageView> attachments;
		// 	// if (bEnableMSAA) {
		// 	//     attachments.push_back(colorImageView_msaa);//for MSAA
		// 	//     attachments.push_back(depthImageView);//for model
		// 	//     attachments.push_back(swapChainImageViews[i]);
		// 	// }
		// 	// else {
		// 		attachments.push_back(swapChainImageViews[i]);
		// 		//attachments.push_back(depthImageView);//for model
		// 	//}

		// 	VkFramebufferCreateInfo framebufferInfo{};
		// 	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		// 	framebufferInfo.renderPass = renderPass;
		// 	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		// 	framebufferInfo.pAttachments = attachments.data();//swapChainImageViews, 类型是VkImageView
		// 	framebufferInfo.width = swapChainExtent.width;
		// 	framebufferInfo.height = swapChainExtent.height;
		// 	framebufferInfo.layers = 1;

		// 	result = vkCreateFramebuffer(LOGICAL_DEVICE, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
		// 	if (result != VK_SUCCESS) throw std::runtime_error("failed to create framebuffer!");
		// 	REPORT("vkCreateFrameBuffer");
		// }
	}

	void CreateDescriptorPool(){
		std::vector<VkDescriptorType> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
		wxjCreateDescriptorPool(descriptorTypes);
	}
    void CreateDescriptorSetLayout(){
		std::vector<VkDescriptorType> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
		std::vector<VkShaderStageFlagBits> shaderStageFlagBits{VK_SHADER_STAGE_VERTEX_BIT};
		wxjCreateDescriptorSetLayout(descriptorTypes, shaderStageFlagBits);
	}
    void CreateDescriptorSets(){
		std::vector<VkDescriptorType> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
		wxjCreateDescriptorSets(descriptorTypes);
	}

	void CreateGraphicsPipeline(){
		wxjCreateGraphicsPipeline(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
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
		
	}

};

int main(){
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