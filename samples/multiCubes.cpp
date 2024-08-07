/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */

#include "..\\framework\\include\\application.h"
#include "object.h"
#define TEST_CLASS_NAME CMultiCubes
class TEST_CLASS_NAME: public CApplication{
public:
	//TODO: need simply draw and update different cubes
	class CCube : public CObject{
	public:
		CCube(){} //TODO: merge load model/texture and record action into CObject...
	};

	CCube cubes[2];

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -8.5f, -8.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		swapchain.bEnableMSAA = true;//!To enable MSAA, make sure it has depth test (call addDepthAttachment())
		swapchain.bEnableDepthTest = true; 

		//multiple models, multiple vertex buffers, multiple index buffers
		std::vector<std::string> modelNames = {"cube.obj", "hallway.obj"}; //"viking_room.obj"
		for(unsigned int i = 0; i < 2; i++)
			cubes[i].Init((CApplication*)this, modelNames[i], i);

		//{
			//cubes[i].id = i;
			//modelManager.LoadObjModel(modelNames[i], cubes[i].vertices3D, cubes[i].indices3D);
			//cubes[i].Init(renderer, renderProcess.graphicsPipelineLayout, graphicsDescriptorManager.descriptorSets);
			
			//renderer.CreateVertexBuffer<Vertex3D>(cubes[i].vertices3D); //the first buffer index is 0(so renderer will use object id to access buffer)
			//renderer.CreateIndexBuffer(cubes[i].indices3D);
		//}

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("viking_room.png", usage, renderer.commandPool);
		textureManager.CreateTextureImage("fur.jpg", 		 usage, renderer.commandPool);

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

		//Uniform List:
		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);

		CDescriptorManager::createDescriptorPool();

		CGraphicsDescriptorManager::createDescriptorSetLayout();
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout();
	
		graphicsDescriptorManager.createDescriptorSets();
		for(int i = 0; i < 2; i++){
			cubes[i].CreateTextureDescriptorSets(
				textureManager.textureImages[cubes[i].GetID()], 
				CGraphicsDescriptorManager::descriptorPool,
				CGraphicsDescriptorManager::textureDescriptorSetLayout,
				CGraphicsDescriptorManager::textureSamplers[0],
				CGraphicsDescriptorManager::CheckMVP()
				);
		}
	
		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout); //set = 0
		dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1
		//!!!Different cube can share the same texture descriptor.
		//suppose we have 100 objects, 100 different textures. cube x 50, sphere x 50. How many texture layouts? How many texture descriptor?
		//obviously, every objects need a different texture, so bind with objectId
		//but for layout, can use one. That means texture layout should be object property, while the descriptor set(associate with image) should be cube[i]/sphere[i] bound

		//each object can have muti texture image, multi descriptor set(when creating descritpor set, need a sampler)
		//all objects share the same descriptor pool and descriptor layout, they are universal
		//sampler should also be universal

		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CGraphicsDescriptorManager::mvpUBO.mvpData[1].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//mvpUBO.mvpData[1].model = glm::mat4(1.0f); //identity matrix
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < 2; i++) cubes[i].RecordDrawIndexCmd();
	}

	~TEST_CLASS_NAME(){
		cubes[0].CleanUp();
		cubes[1].CleanUp();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif