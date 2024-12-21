#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMSAA
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		// appInfo.Object.Count = 1;
		// appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"viking_room.obj"});
		// appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		// appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"viking_room.png", false}});
		// appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		// appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleMSAA/vert.spv"});
		// appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleMSAA/frag.spv"});
		// appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {});
		// appInfo.Object.Pipeline.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		CApplication::initialize();

		objectList[0].SetRotation(-135,0,45);
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