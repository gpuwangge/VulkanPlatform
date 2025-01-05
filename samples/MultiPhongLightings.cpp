#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiPhongLightings
class TEST_CLASS_NAME: public CApplication{
public:
	struct CustomUniformBufferObject {
		//glm::vec3 cameraPos;
		glm::vec3 lightPos;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

    void initialize(){
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		CApplication::initialize();

		//objectList[0].SetRotation(-135,0,45); 
		objectList[0].SetPosition(0, 2.0, 0); //reference sphere
		objectList[0].SetScale(0.03, 0.03f, 0.03f);

		objectList[1].SetPosition(0, -103, 0);  //table

		objectList[2].SetScale(0.01f, 0.01f, 0.01f); //light sphere
		//objectList[2].SetScale(0.05f, 0.05f, 0.05f);
	} 

	void update(){
		customUBO.lightPos = glm::vec3(3.0f * sin(durationTime), 2, 3.0f * cos(durationTime));
		//customUBO.cameraPos = glm::vec3(0, 3, -10);// mainCamera.Position;
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		objectList[2].SetPosition(customUBO.lightPos + glm::vec3(0,0,0));
		//objectList[0].SetAngularVelocity(0, 50, 0);

		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
		//objectList[0].Draw();
		//objectList[1].Draw();
	}		
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif