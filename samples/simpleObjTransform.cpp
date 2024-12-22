#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleObjTransform
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		mainCamera.setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 256.0f);
		mainCamera.cameraType = Camera::CameraType::freemove;
		mainCamera.SetPosition(0.0f, 1.2f, 0.0f);
		mainCamera.SetRotation(0.0f, 0.0f, 0.0f);

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