#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiPhongLightings
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lightList.size(); i++) {
			lightList[i].SetLightPosition(
				glm::vec3(0, lightList[i].GetLightPosition().y,0) +
				glm::vec3(2.5 *cos(durationTime * (i+1)), 0, 2.5 *sin(durationTime * (i+1)))
			);
			objectList[2+i].SetPosition(lightList[i].GetLightPosition()); 
		}

		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
	}		
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif