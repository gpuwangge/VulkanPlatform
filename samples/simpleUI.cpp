/************
 * This sample is to test render 2D UI and 3D Object in a single scene
 * *********** */

#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleUI

//CApplication CSkyvision::*m_app;
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	//TODO: need simply draw and update different cubes
	// class CCube : public CObject{
	// public:
	// 	CCube(){} //TODO: merge load model/texture and record action into CObject...
	// };

	// CCube cubes[2];
	
    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -8.5f, -8.5f));
		mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		modelManager.CreateCustomModel3D(vertices3D, indices3D); //create the 0th custom model 3D (CUSTOM3D0)
		appInfo.Object.Count = 4;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"CUSTOM3D0", "cube.obj", "hallway.obj"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0, 1, 2, 0});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {
			{"texture.jpg", false},
			{"viking_room.png", false},
			{"fur.jpg", false},
			});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0, 0, 0, 0});
		appInfo.Shader.Vertex = "simpleUI/vert.spv";
		appInfo.Shader.Fragment = "simpleUI/frag.spv";
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();

		objectList[0].bSticker = true;
	}

	void update(){
		objectList[0].SetVelocity(0, sin(durationTime * 2), 0);
		objectList[1].SetAngularVelocity(50,0,50); //rotation around x and z axis
		objectList[2].SetVelocity(0, 3*sin(durationTime * 2), 0);
		objectList[3].SetVelocity(3*sin(durationTime * 2), 0, 0);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
		objectList[3].Draw();
		//for(int i = 0; i < appInfo.Object.Count; i++) objectList[i].Draw();
	}

	// ~TEST_CLASS_NAME(){
	// 	cubes[0].CleanUp();
	// 	cubes[1].CleanUp();
	// }
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif