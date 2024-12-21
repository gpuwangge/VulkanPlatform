#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMap
class TEST_CLASS_NAME: public CApplication{
public:
	struct CustomUniformBufferObject {
		glm::vec3 lightPos;
		glm::mat4 lightSpace;

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
		// appInfo.Object.Count = 1;
		// appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"viking_room.obj"});
		// appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		// appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"viking_room.png", false}});
		// appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		// appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleShadowMap/vert.spv"});
		// appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"simpleShadowMap/frag.spv"});
		// appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {});
		// appInfo.Object.Pipeline.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		appInfo.Feature.EnableGraphicsMSAA = true;
		
		CApplication::initialize();

		objectList[0].SetRotation(-135,0,45);
		//objectList[0].YawLeft(90,200);
		//objectList[0].RollLeft(90, 200);
	} 

	void update(){
		// float near_plane = 1.0f, far_plane = 7.5f;
		// glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane); 
		// glm::mat4 lightView = glm::lookAt(
		// 	glm::vec3(-2.0f, 4.0f, -1.0f), 
		// 	glm::vec3( 0.0f, 0.0f,  0.0f),  
		// 	glm::vec3( 0.0f, 1.0f,  0.0f));  
 
		customUBO.lightPos = glm::vec3(0.75f * sin(durationTime * 3), 0.75f * sin(durationTime * 2), 0.75f * sin(durationTime * 1));

		// Matrix from light's point of view
		float lightFOV = 10.0f; //45.0f;
		float zNear = 0.0f; //1.0f
		float zFar = 5.0f; //96.0f
		glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar);
		glm::mat4 depthViewMatrix = glm::lookAt(customUBO.lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0f);

		customUBO.lightSpace = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
		
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		//objectList[0].SetAngularVelocity(0, 0, 50);
		//objectList[0].SetVelocity(sin(durationTime * 2), 0, 0);

		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif