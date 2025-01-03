#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleObjTransform
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
		objectList[0].SetScale(0.01,0.01,0.01); //table
		objectList[1].SetScale(0.003,0.003,0.003); //sphere

		objectList[1].SetPosition(0,1.25,0.75);
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif