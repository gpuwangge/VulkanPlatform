#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleCubemap
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		//mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
		mainCamera.setPosition(glm::vec3(0, 0, 0));
		mainCamera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		mainCamera.setPerspective(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		appInfo.Object.Count = 1;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"cube.obj"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"output_skybox.png", false}});//texture.jpg
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Shader.Vertex = "simpleCubemap/vert.spv";
		appInfo.Shader.Fragment = "simpleCubemap/frag.spv";	
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableCubemap = true;
		//appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();
	}

	void update(){
		//objectList[0].SetAngularVelocity(50,50,50);
		//NeedToExit = true;
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif