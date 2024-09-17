#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleTriangle
//a blue triangle on the screen
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		appInfo.Object.Count = 1;
		appInfo.Shader.Vertex = "simpleTriangle/vert.spv";
		appInfo.Shader.Fragment = "simpleTriangle/frag.spv";
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
#include "..\\windowsFramework\\include\\main.hpp"
#endif

