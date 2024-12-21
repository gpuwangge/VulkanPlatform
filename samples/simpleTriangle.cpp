#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleTriangle
//a blue triangle on the screen
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		//appInfo.Object.Count = 1;
		//appInfo.Shader.Vertex = "simpleTriangle/vert.spv";
		//appInfo.Shader.Fragment = "simpleTriangle/frag.spv";
		// appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleTriangle/vert.spv"});
		// appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleTriangle/frag.spv"});
		// appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {});
		// appInfo.Object.Pipeline.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw(3);
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

