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
		mainCamera.setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 256.0f);
		//mainCamera.cameraType = Camera::CameraType::freemove;
		mainCamera.SetPosition(0.0f, 5.0f, -10.0f);

		//mainCamera.SetPosition(0.0f, -8.5f, -8.5f);
		//mainCamera.SetRotation(45.0f, 0.0f, 0.0f);
		//mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		appInfo.Object.Count = 3;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"cube.obj"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0, 0, 0});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {
			{"viking_room.png", false},
			{"fur.jpg", false},
			{"skin.jpg", false}
			});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0, 1, 2});
		//appInfo.Shader.Vertex = "multiCubes/vert2.spv";
		//appInfo.Shader.Fragment = "multiCubes/frag2.spv";
		appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"multiCubes/vert1.spv", "multiCubes/vert2.spv"});
		appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"multiCubes/frag1.spv", "multiCubes/frag2.spv"});
		appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {});
		appInfo.Object.Pipeline.List = std::make_unique<std::vector<int>>(std::vector<int> {1,0,1});
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();
		objectList[0].MoveLeft(5, 100);
		objectList[2].MoveRight(5, 100);
	}

	void update(){
		objectList[0].SetVelocity(0, 3*sin(durationTime * 2), 0);
		objectList[1].SetAngularVelocity(50,0,50); //rotation around x and z axis
		objectList[2].SetVelocity(0, 3*sin(durationTime * 4), 0);
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