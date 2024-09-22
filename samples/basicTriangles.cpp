#include "..\\framework\\include\\application.h"
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

	std::vector<std::pair<std::string, bool>> textureNames = {{"texture.jpg", false}}; 
	std::vector<int> textureList = {0};
	std::vector<int> modelList = {0}; //this 0 means vertex3D, not model

	void initialize(){
		appInfo.Object.Count = 1;
		appInfo.Buffer.GraphicsVertex.Vertices3D = &vertices3D; 
		appInfo.Buffer.GraphicsVertex.Indices3D = &indices3D;
		appInfo.Object.Model.List = &modelList;
		appInfo.Object.Texture.Names = &textureNames;
		appInfo.Object.Texture.List = &textureList;
		appInfo.Shader.Vertex = "basicTriangles/vert.spv";
		appInfo.Shader.Fragment = "basicTriangles/frag.spv";
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		CApplication::initialize();
	}

	void update(){
		customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, 0.0f, (cos(durationTime) + 1.0f) / 2.0f};
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);
		
		//objectList[0].AngularVelocity = glm::vec3(20,0,0);//Pitch, Yaw, Roll
		//objectList[0].Velocity.z = 0.5;
		objectList[0].Velocity.z = 0.5 * sin(durationTime * 2); //positive number will move to z direction (object move forward )
		objectList[0].Velocity.x = 0.5 * sin(durationTime * 2); //positive number will move to y direction (object move rightward )
		objectList[0].Velocity.y = 0.5 * sin(durationTime * 2); //positive number will move to z direction (object move downward )
		
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

