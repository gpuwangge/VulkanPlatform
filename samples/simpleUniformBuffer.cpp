#include "..\\framework\\include\\application.h"
#include "object.h"
#define TEST_CLASS_NAME CSimpleUniformBuffer
class TEST_CLASS_NAME: public CApplication{
//a changeing color triangle on the screen
public:
	struct CustomUniformBufferObject {
		glm::vec4 color;

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

	std::string vertexShader = "simpleUniformBuffer/vert.spv";
	std::string fragmentShader = "simpleUniformBuffer/frag.spv";

	void initialize(){
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		Activate_Uniform_Graphics_Custom(sizeof(CustomUniformBufferObject), CustomUniformBufferObject::GetBinding());
		Activate_Pipeline(); 

		object.Register((CApplication*)this, -1, -1, 0); //no texture, no model, id=0

		CApplication::initialize();
	}

	void update(){
		//printf("%f\n", durationTime);
		customUBO.color = {(sin(durationTime*3) + 1.0f) / 2.0f, (cos(durationTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw(3);
	}	
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

