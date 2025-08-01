#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMapHard
class TEST_CLASS_NAME: public CApplication{
/*********
 * Implementation of Shadow Map
 * Simple scenario: A single light source casting one shadow on a plane
 * Use two renderpasses: one for shadowmap, one for main scene
 * Use hardware depth bias (vkCmdSetDepthBias)
 */
public:
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
		appInfo.RenderMode = renderer.GRAPHICS_SHADOWMAP; //two renderpasses: shadowmap and main scene

		// appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		// appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				//glm::vec3(lights[i].GetLightPosition().x, lights[i].GetLightPosition().y,lights[i].GetLightPosition().z)
				glm::vec3(2.5 *cos(elapseTime * (i+1)), lights[i].GetLightPosition().y, 2.5 *sin(elapseTime * (i+1)))
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition()); 
		}

		lightCamera.SetPosition(lights[0].GetLightPosition());

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassShadowmap(){
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 1.75f); (self-shadowing)
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 2.0f, 0.0f, 2.5f);
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 2.0f, 0.01f, 2.5f); //need enable clamp
		vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 6.0f);

		for(int i = 0; i < objects.size()-1; i++) {
			//if(i == 2) continue; //dont draw the light ball in shadowmap
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