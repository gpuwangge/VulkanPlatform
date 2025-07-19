#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CHardwareDepthBias
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};

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
		appInfo.RenderMode = renderer.GRAPHICS_SHADOWMAP; //two renderpasses: shadowmap and main scene

		modelManager.CreateCustomModel3D(vertices3D, indices3D); //create the 0th custom model 3D (CUSTOM3D0)

		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				glm::vec3(lights[i].GetLightPosition().x, lights[i].GetLightPosition().y,lights[i].GetLightPosition().z)
				//glm::vec3(lights[i].GetLightPosition().x, 1+2*sin(durationTime*0.25), lights[i].GetLightPosition().z)
				//glm::vec3(0, 2.5, 0)
				//glm::vec3(2.5 *cos(durationTime * (i+1)), 0, 2.5 *sin(durationTime * (i+1)))
				//glm::vec3(0, 3+0.6*sin(durationTime * (i+1)/2), 0)
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition()); 
		}

		lightCamera.SetPosition(lights[0].GetLightPosition());

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassShadowmap(){
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 1.75f);
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 125, 0.0f, 175);

		for(int i = 0; i < objects.size()-1; i++) {
			if(i == 2) continue; //dont draw the light ball in shadowmap
			//objects[i].m_graphics_pipeline_id = 1; //same object, first subpass use gid=2, second subpass use gid=0
			objects[i].Draw(0, 0, true);//draw objects with gid=0, shadowmap pipeline
		}
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) 
			objects[i].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif