/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */

#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CMultiCubes
class TEST_CLASS_NAME: public CApplication{
public:
	//TODO: need create vectors for different cubes
	class CCube : public CObject{
	public:
		CCube(){} //TODO: merge load model/texture and record action into CObject...
	};

	CCube cubes[2];

	std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  },  { 1.0f, 0 } };

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -8.5f, -8.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		swapchain.bEnableMSAA = true;//!To enable MSAA, make sure it has depth test (call addDepthAttachment())
		swapchain.bEnableDepthTest = true; 

		//multiple models, multiple vertex buffers, multiple index buffers
		std::vector<std::string> modelNames = {"cube.obj", "hallway.obj"}; //"viking_room.obj"
		for(unsigned int i = 0; i < 2; i++){
			cubes[i].id = i;
			modelManager.LoadObjModel(modelNames[i], cubes[i].vertices3D, cubes[i].indices3D);
			renderer.CreateVertexBuffer<Vertex3D>(cubes[i].vertices3D); //the first buffer index is 0(so renderer will use object id to access buffer)
			renderer.CreateIndexBuffer(cubes[i].indices3D);
		}

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		//TODO: Add texture for different objects
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureImages[0].CreateTextureImage("viking_room.png", usage, renderer.commandPool);
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

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

		shaderManager.CreateShader("multiCubes/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("multiCubes/frag.spv", shaderManager.FRAG);

		descriptors[0].addMVPUniformBuffer();
		descriptors[0].addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets(&textureImages);

		renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		descriptors[0].mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		descriptors[0].mvpUBO.mvpData[1].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//descriptors[0].mvpUBO.mvpData[1].model = glm::mat4(1.0f); //identity matrix
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)

		drawObject(0); //draw object id = 0
		drawObject(1); //draw object id = 1
		
		END_GRAPHICS_RECORD
	}

	void drawObject(int objectId){
		//If shader is the same, can reuse the same descriptor for different objects.
		//The offset index can be different than object id. Different offset index means different MVP uniforms.
		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, descriptors[0].descriptorSets, objectId);
		
		renderer.BindVertexBuffer(objectId);
		renderer.BindIndexBuffer(objectId);
		renderer.DrawIndexed(cubes[objectId].indices3D);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif