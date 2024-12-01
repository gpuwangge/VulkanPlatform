/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */

#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiCubes

//CApplication CSkyvision::*m_app;
class TEST_CLASS_NAME: public CApplication{
public:
	//TODO: need simply draw and update different cubes
	// class CCube : public CObject{
	// public:
	// 	CCube(){} //TODO: merge load model/texture and record action into CObject...
	// };

	// CCube cubes[2];
	
    void initialize(){
		mainCamera.SetPosition(0.0f, -8.5f, -8.5f);
		mainCamera.SetRotation(45.0f, 0.0f, 0.0f);
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		appInfo.Object.Count = 2;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"cube.obj", "hallway.obj"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0, 1});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {
			{"viking_room.png", false},
			{"fur.jpg", false},
			});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0, 1});
		appInfo.Shader.Vertex = "multiCubes/vert.spv";
		appInfo.Shader.Fragment = "multiCubes/frag.spv";
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();
	}

	void update(){
		objectList[0].SetVelocity(0, 0, 3*sin(durationTime * 2));
		objectList[1].SetAngularVelocity(50,0,50); //rotation around x and z axis
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < appInfo.Object.Count; i++) objectList[i].Draw();
	}

	// ~TEST_CLASS_NAME(){
	// 	cubes[0].CleanUp();
	// 	cubes[1].CleanUp();
	// }
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif