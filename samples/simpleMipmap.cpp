#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMipmap
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		mainCamera.type = Camera::CameraType::firstperson;
		mainCamera.setPosition(glm::vec3(0.0f, -0.8f, 0.0f));
		mainCamera.setRotation(glm::vec3(0.0f, 90.00001f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		appInfo.Object.Count = 1;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"hallway.obj"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"checkerboard_marble.jpg", true}});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Shader.Vertex = "simpleMipmap/vert.spv";
		appInfo.Shader.Fragment = "simpleMipmap/frag.spv";	
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