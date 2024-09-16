#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CFurMark
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
 
	struct CustomUniformBufferObject {
		//glm::vec3 color;
        glm::vec2 u_resolution;
	    float u_time;

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

	std::vector<std::pair<std::string, bool>> textureNames = {
		{"fur.jpg", false},
		{"noise.png", false},
		{"wall.jpg", false}
	}; 
	std::vector<int> textureList = {0}; //object_0 uses 3 texture(in 3 sampler), how to describe?
	std::vector<int> modelList = {0}; 

	void initialize(){
		appInfo.Object.Count = 1;
		appInfo.Object.Model.List = &modelList;
		appInfo.Object.Texture.Names = &textureNames;
		appInfo.Object.Texture.List = &textureList;
		appInfo.Shader.Vertex = "furMark/vert.spv";
		appInfo.Shader.Fragment = "furMark/frag.spv";
		appInfo.Uniform.GraphicsSampler.Enable = true;
		appInfo.Uniform.GraphicsSampler.Count = textureNames.size();
		appInfo.Uniform.GraphicsSampler.UseMultiSamplerForOneObject = true;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Uniform.GraphicsCustom.Enable = true;
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		appInfo.Buffer.GraphicsVertex.Enable = true;
		appInfo.Buffer.GraphicsVertex.Vertices3D = &vertices3D; 
		appInfo.Buffer.GraphicsVertex.Indices3D = &indices3D;

		//CSupervisor::VertexShader = vertexShader;
		//CSupervisor::FragmentShader = fragmentShader;
		//Activate_Uniform_Graphics_Sampler(textureNames.size());
		//Activate_Uniform_Graphics_MVP();
		//Activate_Uniform_Graphics_Custom(sizeof(CustomUniformBufferObject), CustomUniformBufferObject::GetBinding());
		//Activate_Buffer_Graphics_Vertex(vertices3D, indices3D);
		//Activate_Texture(&textureNames);
		//Activate_Pipeline();

		//object.Register((CApplication*)this, INT_MAX);//INT_MAX means use all samplers

		CApplication::initialize();
	}

	void update(){
		//customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, 0.0f, (cos(durationTime) + 1.0f) / 2.0f};
        customUBO.u_time = durationTime;
        customUBO.u_resolution = glm::vec2(windowWidth, windowHeight);
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		//descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw(); 
	}

};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

