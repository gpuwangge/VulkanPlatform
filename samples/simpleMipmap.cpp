#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMipmap
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		mainCamera.cameraType = Camera::CameraType::freemove;
		mainCamera.SetPosition(0.0f, -0.8f, 0.0f);
		mainCamera.SetRotation(0.0f, 90.0f, 0.0f);
		//mainCamera.YawLeft(90, 100);
		//mainCamera.RollLeft(90, 100);
		//mainCamera.PitchUp(90, 100);

		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
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