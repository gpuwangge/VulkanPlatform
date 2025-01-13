#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePhongLighting
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lightList.size(); i++) {
			lightList[i].SetLightPosition(glm::vec3(1.5f * cos(durationTime * 1.2), 1.5f * cos(durationTime * 0.85), 1.5f * cos(durationTime * 1)));
			objectList[1+i].SetPosition(lightList[i].GetLightPosition()); 
		}

		//objectList[0].SetAngularVelocity(0, 50, 0);
		
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
	}		
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif