#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiPhongLightings
#define LIGHT_NUM 3
class TEST_CLASS_NAME: public CApplication{
public:
	// struct CustomUniformBufferObject {
	// 	alignas(16) LightAttribute lights[LIGHT_NUM];
	// 	alignas(16) glm::vec4 cameraPos; 

	// 	static VkDescriptorSetLayoutBinding GetBinding(){
	// 		VkDescriptorSetLayoutBinding binding;
	// 		binding.binding = 0;//not important, will be reset
	// 		binding.descriptorCount = 1;
	// 		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// 		binding.pImmutableSamplers = nullptr;
	// 		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	// 		return binding;
	// 	}
	// };
	// CustomUniformBufferObject customUBO{};

	//LightingUniformBufferObject CGraphicsDescriptorManager::lightingUBO;

    void initialize(){
		// std::cout<<"sizeof(CustomUniformBufferObject)="<<sizeof(CustomUniformBufferObject)<<std::endl;
		// appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		// appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();

		CApplication::initialize();

		//objectList[0].SetRotation(-135,0,45); 
		objectList[0].SetPosition(0, 2.0, 0); //reference sphere
		objectList[0].SetScale(0.03, 0.03f, 0.03f);

		objectList[1].SetPosition(0, -103, 0);  //table

		objectList[2].SetScale(0.01f, 0.01f, 0.01f); //light sphere
		objectList[3].SetScale(0.01f, 0.01f, 0.01f); //light sphere
		objectList[4].SetScale(0.01f, 0.01f, 0.01f); //light sphere

		// for(int i = 0; i < LIGHT_NUM; i++){
		// 	customUBO.lights[i].ambientIntensity = 0.5;
		// 	customUBO.lights[i].diffuseIntensity = 2.0f;
		// 	customUBO.lights[i].specularIntensity = 4.0f;
		// }

	} 

	void update(){
		// customUBO.cameraPos = glm::vec4(mainCamera.Position, 0);
		// customUBO.lights[0].lightPos = glm::vec4(glm::vec3(1.5f * sin(durationTime * 3), 2, 1.5f * cos(durationTime * 3)), 0);
		// customUBO.lights[1].lightPos = glm::vec4(glm::vec3(2.8 * cos(durationTime * 2), 2 + 2.8 * cos(durationTime * 2), 2.8 * sin(durationTime * 2)), 0);
		// customUBO.lights[2].lightPos = glm::vec4(glm::vec3(2.2f * cos(durationTime * 1), 2 + 2.2f * sin(durationTime * 1), 0), 0);
		// graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);


		CGraphicsDescriptorManager::m_lightingUBO.cameraPos = glm::vec4(mainCamera.Position, 0);
		//CGraphicsDescriptorManager::m_lightingUBO.lights[0].lightPos += glm::vec4(glm::vec3(1.5f*0.000001 * sin(durationTime * 3), 2, 1.5f*0.000001 * cos(durationTime * 3)), 0);
		//CGraphicsDescriptorManager::m_lightingUBO.lights[1].lightPos += glm::vec4(glm::vec3(2.8*0.000001 * cos(durationTime * 2), 2 + 2.8*0.000001 * cos(durationTime * 2), 2.8*0.000001 * sin(durationTime * 2)), 0);
		//CGraphicsDescriptorManager::m_lightingUBO.lights[2].lightPos += glm::vec4(glm::vec3(2.2*0.000001 * cos(durationTime * 1), 2 + 2.2*0.000001 * sin(durationTime * 1), 0), 0);
		float scale = 0.001f;
		CGraphicsDescriptorManager::m_lightingUBO.lights[0].lightPos += glm::vec4(glm::vec3(1.5*scale * sin(durationTime * 3), 0, 1.5*scale * cos(durationTime * 3)), 0);
		CGraphicsDescriptorManager::m_lightingUBO.lights[1].lightPos += glm::vec4(glm::vec3(2.8*scale * cos(durationTime * 2), 2.8*scale * cos(durationTime * 2), 2.8*scale * sin(durationTime * 2)), 0);
		CGraphicsDescriptorManager::m_lightingUBO.lights[2].lightPos += glm::vec4(glm::vec3(2.2*scale * cos(durationTime * 1), 2.2*scale * sin(durationTime * 1), 0), 0);
		CGraphicsDescriptorManager::updateLightingUniformBuffer(renderer.currentFrame, durationTime);

		objectList[2].SetPosition(glm::vec3(CGraphicsDescriptorManager::m_lightingUBO.lights[0].lightPos) + glm::vec3(0,0,0));
		objectList[3].SetPosition(glm::vec3(CGraphicsDescriptorManager::m_lightingUBO.lights[1].lightPos) + glm::vec3(0,0,0));
		objectList[4].SetPosition(glm::vec3(CGraphicsDescriptorManager::m_lightingUBO.lights[2].lightPos) + glm::vec3(0,0,0));

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