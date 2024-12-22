/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */

#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiCubes

//CApplication CSkyvision::*m_app;
class TEST_CLASS_NAME: public CApplication{
public:
	//TODO: need simply draw and update different cubes
	// class CCube : public CObject{
	// public:
	// 	CCube(){} //TODO: merge load model/texture and record action into CObject...
	// };

	// CCube cubes[2];
	
    void initialize(){
		mainCamera.setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 256.0f);
		mainCamera.SetPosition(0.0f, 5.0f, -10.0f);

		CApplication::initialize();
		objectList[0].MoveLeft(5, 100);
		objectList[2].MoveRight(5, 100);
	}

	void update(){
		objectList[0].SetVelocity(0, 3*sin(durationTime * 2), 0);
		objectList[1].SetAngularVelocity(50,0,50); //rotation around x and z axis
		objectList[2].SetVelocity(0, 3*sin(durationTime * 4), 0);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
	}

	// ~TEST_CLASS_NAME(){
	// 	cubes[0].CleanUp();
	// 	cubes[1].CleanUp();
	// }
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif