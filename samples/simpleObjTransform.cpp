#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleObjTransform
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		//mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
		//mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
		//mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		mainCamera.cameraType = Camera::CameraType::freemove;
		appInfo.Object.Count = 1;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"BaseMesh_Female.obj"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"female_ao.psd", false}});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Shader.Vertex = "simpleObjTransform/vert.spv";
		appInfo.Shader.Fragment = "simpleObjTransform/frag.spv";	
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();
	}

	void update(){
		//objectList[0].SetAngularVelocity(50,50,50);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif