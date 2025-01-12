#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiPhongLightings
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < GetLightSize(); i++) {
			SetLightPosition(i,
				glm::vec3(0,GetLightPosition(i).y,0) +
				glm::vec3( 2.5 *cos(durationTime * (i+1)), 0, 2.5 *sin(durationTime * (i+1)))
			);
			objectList[2+i].SetPosition(GetLightPosition(i)); 
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