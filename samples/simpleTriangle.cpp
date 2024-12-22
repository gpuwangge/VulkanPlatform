#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleTriangle
//a blue triangle on the screen
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
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

