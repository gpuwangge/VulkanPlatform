/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */

#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CMultiCubes

//CApplication CSkyvision::*m_app;
class TEST_CLASS_NAME: public CApplication{
public:
	//TODO: need simply draw and update different cubes
	class CCube : public CObject{
	public:
		CCube(){} //TODO: merge load model/texture and record action into CObject...
	};

	CCube cubes[2];

	std::vector<std::string> modelNames = {"cube.obj", "hallway.obj"}; //"viking_room.obj"
	std::vector<std::pair<std::string, bool>> textureNames = {{"viking_room.png", false}, {"fur.jpg", false}}; 
	std::string vertexShader = "multiCubes/vert.spv";
	std::string fragmentShader = "multiCubes/frag.spv";

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -8.5f, -8.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		CSkyvision::Register((CApplication*)this);
		CSkyvision::ActivateMSAA();
		CSkyvision::LoadResources(modelNames, textureNames, vertexShader, fragmentShader);

		for(unsigned int i = 0; i < 2; i++)
			cubes[i].Register((CApplication*)this, i, i, i);

	
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
		//CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//CGraphicsDescriptorManager::mvpUBO.mvpData[1].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		cubes[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		cubes[1].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		//mvpUBO.mvpData[1].model = glm::mat4(1.0f); //identity matrix
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < 2; i++) cubes[i].Draw();
	}

	// ~TEST_CLASS_NAME(){
	// 	cubes[0].CleanUp();
	// 	cubes[1].CleanUp();
	// }
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif