#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiPhongLightings
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	} 

	void update(){
		CGraphicsDescriptorManager::m_lightingUBO.cameraPos = glm::vec4(mainCamera.Position, 0);
		CGraphicsDescriptorManager::updateLightingUniformBuffer(renderer.currentFrame, durationTime);

		for(int i = 0; i < LightCount; i++) {
			CGraphicsDescriptorManager::m_lightingUBO.lights[i].lightPos = 
				glm::vec4(0,CGraphicsDescriptorManager::m_lightingUBO.lights[i].lightPos.y,0,0) + 
				glm::vec4(glm::vec3( 2.5 *cos(durationTime * (i+1)), 0, 2.5 *sin(durationTime * (i+1))), 0); 
			objectList[2+i].SetPosition(glm::vec3(CGraphicsDescriptorManager::m_lightingUBO.lights[i].lightPos) + glm::vec3(0,0,0)); //sphere put on light's location
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