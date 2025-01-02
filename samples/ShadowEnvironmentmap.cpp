#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CShadowEnvironmentmap
class TEST_CLASS_NAME: public CApplication{
public:
	struct CustomUniformBufferObject {
		//uniform for vertex shader
		glm::vec3 lightPos;
	 	glm::mat4 lightSpace;

		//uniform for fragment shader
		glm::vec3 cameraPos;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

	// struct CustomUniformBufferObject {
	// 	glm::vec3 lightPos;
	// 	glm::mat4 lightSpace;

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

    void initialize(){
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();

		CApplication::initialize();
		for(int i = 1; i < objectList.size()-1; i++)
			objectList[i].SetScale(0.01f, 0.01f, 0.01f);

		objectList[0].SetScale(0.05f, 0.05f, 0.05f);
		objectList[0].SetPosition(0, -7, 0); //table

		objectList[1].SetPosition(-1, 0, 0);
		objectList[2].SetPosition(1, 0, 0);
		objectList[3].SetPosition(0, -1, 0);
		objectList[4].SetPosition(0, 1, 0);
		objectList[5].SetPosition(0, 0, -1);
		objectList[6].SetPosition(0, 0, 1);

		objectList[7].SetAngularVelocity(10,10,10);
	}

	void update(){
		//update uniform for vertex shader
		//customUBO.lightPos = glm::vec3(0.75f * sin(durationTime * 3), 0.75f * sin(durationTime * 2), 0.75f * sin(durationTime * 1));
		customUBO.lightPos = glm::vec3(0,5,0);
		float lightFOV = 10.0f; //45.0f;
		float zNear = 0.0f; //1.0f
		float zFar = 5.0f; //96.0f
		glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar); // Matrix from light's point of view
		glm::mat4 depthViewMatrix = glm::lookAt(customUBO.lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0f);
		customUBO.lightSpace = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		//update uniform for fragment shader
		customUBO.cameraPos = mainCamera.Position;
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

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