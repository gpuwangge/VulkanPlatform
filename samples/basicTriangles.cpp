#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CBasicTriangles
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	struct CustomUniformBufferObject {
		glm::vec3 color;

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

	std::vector<std::pair<std::string, bool>> textureNames = {{"texture.jpg", false}}; 
	std::string vertexShader = "basicTriangles/vert.spv";
	std::string fragmentShader = "basicTriangles/frag.spv";

	void initialize(){
		CSupervisor::Register((CApplication*)this);
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		CSupervisor::Activate_Uniform_Graphics_Sampler();
		CSupervisor::Activate_Uniform_Graphics_MVP();
		CSupervisor::Activate_Uniform_Graphics_Custom(sizeof(CustomUniformBufferObject), CustomUniformBufferObject::GetBinding());
		CSupervisor::Activate_Buffer_Graphics_Vertex(vertices3D, indices3D);
		CSupervisor::Activate_Texture(&textureNames);
		CSupervisor::Activate_Pipeline();
		
		object.Register((CApplication*)this); 
		
		CApplication::initialize();
	}

	void update(){
		customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, 0.0f, (cos(durationTime) + 1.0f) / 2.0f};
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		//graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		object.SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

