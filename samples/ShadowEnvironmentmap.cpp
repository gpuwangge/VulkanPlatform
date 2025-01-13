#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CShadowEnvironmentmap
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	}

	void update(){
		// for(int i = 0; i < lightList.size(); i++) {
		// }

		objectList[1].SetVelocity(glm::cross(objectList[1].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[2].SetVelocity(glm::cross(objectList[2].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[3].SetVelocity(glm::cross(objectList[3].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[4].SetVelocity(glm::cross(objectList[4].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[5].SetVelocity(glm::cross(objectList[5].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
		objectList[6].SetVelocity(glm::cross(objectList[6].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));

		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif