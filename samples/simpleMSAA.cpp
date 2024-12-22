#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMSAA
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
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