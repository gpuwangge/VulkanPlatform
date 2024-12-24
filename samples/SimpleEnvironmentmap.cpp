#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleEnvironmentmap
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
		objectList[0].SetScale(0.1f, 0.1f, 0.1f);
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
		objectList[1].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif