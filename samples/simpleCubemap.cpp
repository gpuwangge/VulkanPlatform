#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleCubemap
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		mainCamera.cameraType = Camera::CameraType::freemove;
		mainCamera.SetPosition(0, 0, 0);
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