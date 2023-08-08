#include "..\\framework\\include\\vulkanBase.h"

#define TEST_CLASS_NAME CBasicTriangles
class TEST_CLASS_NAME: public CVulkanBase{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
	};

	/************
	buffer size: 6*4=24（byte）
	************/
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	struct CustomUniformBufferObject {
		glm::vec3 color;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

	void initialize(){
		//Create bufferss
		wxjCreateVertexBuffer<Vertex3D>(vertices3D);
		
		wxjCreateIndexBuffer(indices3D);
		wxjCreateCommandBuffer();

		//Create texture resource
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		wxjCreateImage_texture("../textures/texture.jpg", usage, textureImageBuffer, texWidth, texHeight);
		wxjCreateImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, OUT textureImageView);


		wxjCreateSwapChainImagesAndImageViews();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		wxjCreateFramebuffers();

		//Create shader resource
		shaderManager.InitVertexShader("../shaders/basicTriangles/vert.spv");
		shaderManager.InitFragmentShader("../shaders/basicTriangles/frag.spv");

		//Create Descriptors
		descriptor.addImageSamplerUniformBuffer(mipLevels);
		descriptor.addMVPUniformBuffer();
		descriptor.addCustomUniformBuffer(sizeof(CustomUniformBufferObject));
		descriptor.createDescriptorPool();
		VkDescriptorSetLayoutBinding customBinding = CustomUniformBufferObject::GetBinding();
		descriptor.createDescriptorSetLayout(&customBinding);
		descriptor.createDescriptorSets(&textureImageView);

		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule, 
			descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		customUBO.color = {0.0, (sin(durationTime) + 1.0f) / 2.0f, (cos(durationTime) + 1.0f) / 2.0f};
		descriptor.updateCustomUniformBuffer<CustomUniformBufferObject>(currentFrame, durationTime, customUBO);

		descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordCommandBuffer(){
		wxjBeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		wxjBeginRenderPass(clearValues);

		wxjBindPipeline();
		wxjSetViewport();
		wxjSetScissor();
		wxjBindVertexBuffer();
		wxjBindIndexBuffer();
		wxjBindDescriptorSets();
		wxjDrawIndexed(indices3D);

		wxjEndRenderPass();
		wxjEndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"


