#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMapMultiple
class TEST_CLASS_NAME: public CApplication{
/*********
 * Implementation of Shadow Map
 * Simple scenario: multiple lights casting multiple shadows on a plane
 * Use two renderpasses: one for shadowmap, one for main scene
 * Use hardware depth bias (vkCmdSetDepthBias)
 * Use push constant to pass shadowmap renderpass index to vertex shader
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
			objects[2+i].SetPosition(lights[i].GetLightPosition()); //object2<-light0's position; object3<-light1's position;
			lightCamera[i].SetPosition(lights[i].GetLightPosition());
			//lightCamera[i].SetPosition(lights[0].GetLightPosition()); //TODO: set to 0 to test, so both light cameras are at the same position(light0)
			//lightCamera[i].SetPosition(lights[1].GetLightPosition()); //TODO: set to 1 to test, so both light cameras are at the same position(light1)
		}
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassShadowmap(int renderpassIndex){
		IntPushConstants pushConstants;
        pushConstants.value = renderpassIndex; //pass shadowmap renderpass index to device
        renderer.PushConstantToCommand<IntPushConstants>(pushConstants, renderProcess.graphicsPipelineLayouts[0], shaderManager.pushConstantRange); //pipeline0 is for shadowmap
		
		vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 6.0f);

		//object0: table
		//object1: middle big sphere
		//object2: small light sphere0 (light0), because both light cameras are at this position, it should not be drawn
		//object3: small light sphere1 (light1)
		for(int i = 0; i < objects.size(); i++) {
			if(i == 2) continue; //dont draw object2(light0) in shadowmap
			if(i == 3) continue; //dont draw object3(light1) in shadowmap
			objects[i].Draw(0, 0, true); //pipeline0 is for shadowmap
		}
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) 
			objects[i].Draw(); //use object's default pipeline#. In this case it is pipeline2
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif