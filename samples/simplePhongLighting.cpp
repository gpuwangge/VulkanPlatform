#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePhongLighting
#include "object.h"
class TEST_CLASS_NAME: public CApplication{
public:
	struct CustomUniformBufferObject {
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

	CObject object;

	std::vector<std::string> modelNames = {"viking_room.obj"}; 
	std::vector<std::pair<std::string, bool>> textureNames = {{"viking_room.png", false}}; //first: textureName, second: mipmap
	std::string vertexShader = "simplePhoneLighting/vert.spv";
	std::string fragmentShader = "simplePhoneLighting/frag.spv";	

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
		customUBO.lightPos = glm::vec3(0.75f * sin(durationTime * 3), 0.75f * sin(durationTime * 2), 0.75f * sin(durationTime * 1));
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		//CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f),  glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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