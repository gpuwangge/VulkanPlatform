#include "..\\framework\\include\\application.h"
#include "object.h"
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

	CObject object;

	std::vector<std::string> modelNames = {"viking_room.obj"}; 
	std::vector<std::pair<std::string, bool>> textureNames = {{"viking_room.png", false}}; //first: textureName, second: mipmap
	std::string vertexShader = "simpleShadowMap/vert.spv";
	std::string fragmentShader = "simpleShadowMap/frag.spv";

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		Activate_Uniform_Graphics_Sampler();
		Activate_Uniform_Graphics_MVP();
		Activate_Uniform_Graphics_Custom(sizeof(CustomUniformBufferObject), CustomUniformBufferObject::GetBinding());
		Activate_Feature_Graphics_MSAA();
		Activate_Buffer_Graphics_Vertex(modelNames);
		Activate_Texture(&textureNames);
		Activate_Pipeline();

		object.Register((CApplication*)this);

		CApplication::initialize();
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

		//graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f),  glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		object.SetRotation(glm::rotate(glm::mat4(1.0f),  glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif