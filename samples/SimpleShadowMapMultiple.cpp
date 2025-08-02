#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMapMultiple
class TEST_CLASS_NAME: public CApplication{
/*********
 * Implementation of Shadow Map
 * Simple scenario: multiple lights casting multiple shadows on a plane
 * Use two renderpasses: one for shadowmap, one for main scene
 * Use hardware depth bias (vkCmdSetDepthBias)
 */
public:

	void initialize(){
		appInfo.RenderMode = renderer.GRAPHICS_SHADOWMAP; //two renderpasses: shadowmap and main scene

		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				//glm::vec3(lights[i].GetLightPosition().x, lights[i].GetLightPosition().y,lights[i].GetLightPosition().z)
				//glm::vec3(lights[i].GetLightPosition().x, 1+1.5*sin(elapseTime*1.5), lights[i].GetLightPosition().z)
				glm::vec3(2.5 *cos(elapseTime * (i+1)), lights[i].GetLightPosition().y, 2.5 *sin(elapseTime * (i+1)))
				//glm::vec3(0, 3+0.6*sin(elapseTime * (i+1)/2), 0)
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition());
			lightCamera[i].SetPosition(lights[i].GetLightPosition());
		}
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