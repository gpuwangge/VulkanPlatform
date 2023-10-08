#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CFurMark
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
 
	struct CustomUniformBufferObject {
		//glm::vec3 color;
        glm::vec2 u_resolution;
	    float u_time;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

	void initialize(){
		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateCommandBuffers();

        textureImages.resize(3);
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureImages[0].CreateTextureImage("fur.jpg", usage, renderer.commandPool);	
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        textureImages[1].CreateTextureImage("noise.png", usage, renderer.commandPool);	
		textureImages[1].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        textureImages[2].CreateTextureImage("wall.jpg", usage, renderer.commandPool);	
		textureImages[2].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.CreateVertexShader("furMark/vert.spv");
		//shaderManager.CreateFragmentShader("furMark/frag.spv");
		shaderManager.CreateShader("furMark/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("furMark/frag.spv", shaderManager.FRAG);

        descriptors[0].textureSamplers.resize(3);
		descriptors[0].addImageSamplerUniformBuffer(textureImages[0].mipLevels);
        descriptors[0].addImageSamplerUniformBuffer(textureImages[1].mipLevels);
        descriptors[0].addImageSamplerUniformBuffer(textureImages[2].mipLevels);
		//descriptor.addMVPUniformBuffer();
		descriptors[0].addCustomUniformBuffer(sizeof(CustomUniformBufferObject));
		descriptors[0].createDescriptorPool();
		VkDescriptorSetLayoutBinding customBinding = CustomUniformBufferObject::GetBinding();
		descriptors[0].createDescriptorSetLayout(&customBinding);
		descriptors[0].createDescriptorSets(textureImages);

		renderProcess.createLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		//customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, 0.0f, (cos(durationTime) + 1.0f) / 2.0f};
        customUBO.u_time = durationTime;
        customUBO.u_resolution = glm::vec2(windowWidth, windowHeight);
		descriptors[0].updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		//descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD

		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.DrawIndexed(indices3D);

		END_GRAPHICS_RECORD
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

