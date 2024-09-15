/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */

#include "..\\framework\\include\\application.h"
#include "object.h"
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
		
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		Activate_Uniform_Graphics_Sampler();
		Activate_Uniform_Graphics_MVP();
		Activate_Feature_Graphics_MSAA();
		Activate_Buffer_Graphics_Vertex(modelNames);
		Activate_Texture(&textureNames);
		Activate_Pipeline();

		for(unsigned int i = 0; i < 2; i++)
			cubes[i].Register((CApplication*)this, i, i, i);


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