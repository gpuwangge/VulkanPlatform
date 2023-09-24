#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CBptpc16Texture
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  },  { 1.0f, 0 } };

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
		mainCamera.setPerspective(90.0f, (float)windowWidth /  (float)windowHeight, 0.1f, 256.0f);

		swapchain.bEnableDepthTest = true;
		swapchain.bEnableMSAA = true; //!To enable MSAA, make sure it has depth test first (call wxjCreateDepthAttachment())
		textureImages[0].bEnableMipMap = true;

		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateCommandBuffers();

		VkImageUsageFlags usage_texture = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureImages[0].imageFormat = VK_FORMAT_R16G16B16A16_UNORM;//VK_FORMAT_R16G16B16A16_UNORM or VK_FORMAT_R16G16B16A16_SFLOAT(need frac_float16())
		textureImages[0].CreateTextureImage("48bpt.png", usage_texture, renderer.commandPool, 16);
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		textureImages[0].generateMipmaps(); 
		
		VkImageUsageFlags usage;
		if(swapchain.bEnableMSAA){
			swapchain.GetMaxUsableSampleCount();
			usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchain.createMSAAImages(VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			swapchain.createMSAAImageViews(VK_IMAGE_ASPECT_COLOR_BIT);
		}

		VkFormat depthFormat = renderProcess.findDepthFormat();
		usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		swapchain.createDepthImages(depthFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		swapchain.createDepthImageViews(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		if(swapchain.bEnableMSAA) imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, swapchain.msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.addDepthAttachment(); //add this function will enable depth attachment(bUseDepthAttachment = true)
		if(swapchain.bEnableMSAA) renderProcess.addColorAttachmentResolve(); //add this function will enable color resolve attachment (bUseColorAttachmentResolve = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateVertexShader("simpleTexture/vert.spv");
		shaderManager.CreateFragmentShader("simpleTexture/frag.spv");

		descriptor.addMVPUniformBuffer();
		descriptor.addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(textureImages);

		renderProcess.createLayout(descriptor.descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordCommandBuffer(){
		RENDER_START

		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.DrawIndexed(indices3D);

		RENDER_END
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif