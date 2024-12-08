#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleObjTransform
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		mainCamera.setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		mainCamera.cameraType = Camera::CameraType::freemove;
		mainCamera.SetPosition(0.0f, 1.2f, 0.0f);
		mainCamera.SetRotation(0.0f, 0.0f, 0.0f);

		//mainCamera.cameraType = Camera::CameraType::lookat;
		//mainCamera.SetPosition(0.0f, 3.0f, -3.0f);
		//mainCamera.SetTargetPosition(0,0,0);

		appInfo.Object.Count = 1;
		//appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"BaseMesh_Female.obj"});
		//appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"Skull.obj"});
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"ElegantChairTable.obj"});
		
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		//appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"female/female_thickness.psd", false}});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"repeat-pattern2.jpg", true}});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Shader.Vertex = "simpleObjTransform/vert.spv";
		appInfo.Shader.Fragment = "simpleObjTransform/frag.spv";	
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();

		objectList[0].SetScale(0.01,0.01,0.01);
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