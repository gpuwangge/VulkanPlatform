#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleEnvironmentmap
class TEST_CLASS_NAME: public CApplication{
public:
	struct CustomUniformBufferObject {
		glm::vec3 cameraPos;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

    void initialize(){
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();

		CApplication::initialize();
		for(int i = 0; i < objectList.size()-1; i++)
			objectList[i].SetScale(0.2f, 0.2f, 0.2f);

		objectList[0].SetPosition(-1, 0, 0) ;
		objectList[1].SetPosition(1, 0, 0) ;
		objectList[2].SetPosition(0, -1, 0) ;
		objectList[3].SetPosition(0, 1, 0) ;
		objectList[4].SetPosition(0, 0, -1) ;
		objectList[5].SetPosition(0, 0, 1) ;

		objectList[6].SetAngularVelocity(10,10,10);
	}

	void update(){
		customUBO.cameraPos = mainCamera.Position;
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		objectList[0].SetVelocity(glm::cross(objectList[0].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[1].SetVelocity(glm::cross(objectList[1].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[2].SetVelocity(glm::cross(objectList[2].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[3].SetVelocity(glm::cross(objectList[3].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objectList[4].SetVelocity(glm::cross(objectList[4].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
		objectList[5].SetVelocity(glm::cross(objectList[5].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));

		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		for(int i = 0; i < objectList.size(); i++) objectList[i].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif