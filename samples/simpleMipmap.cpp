#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMipmap
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		mainCamera.cameraType = Camera::CameraType::freemove;
		mainCamera.SetPosition(0.0f, -0.8f, 0.0f);
		mainCamera.SetRotation(0.0f, 90.0f, 0.0f);
		//mainCamera.YawLeft(90, 100);
		//mainCamera.RollLeft(90, 100);
		//mainCamera.PitchUp(90, 100);

		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		// appInfo.Object.Count = 1;
		// appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"hallway.obj"});
		// appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		// appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"checkerboard_marble.jpg", true}});
		// appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
	
		// appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleMipmap/vert.spv"});
		// appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleMipmap/frag.spv"});
		// appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {});
		// appInfo.Object.Pipeline.List = std::make_unique<std::vector<int>>(std::vector<int> {0});

		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		appInfo.Feature.EnableGraphicsRainbowMipmap = true;
		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif