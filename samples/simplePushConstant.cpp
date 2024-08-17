#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePushConstant
#include "object.h"
#include "supervisor.h"
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
 
	CObject object;

	std::vector<std::pair<std::string, bool>> textureNames = {{"texture.jpg", false}}; //first: textureName, second: mipmap
	std::string vertexShader = "simplePushConstant/vert.spv";
	std::string fragmentShader = "simplePushConstant/frag.spv";

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -1.5f));

		CSupervisor::Register((CApplication*)this);
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		CSupervisor::Activate_Uniform_Graphics_Sampler();
		CSupervisor::Activate_Feature_Graphics_PushConstant(); //Use Push Constant to pass Model matrix
		CSupervisor::Activate_Uniform_Graphics_VP(); //Use VP matrix instead of MVP
		CSupervisor::Activate_Buffer_Graphics_Vertex(vertices3D, indices3D);
		CSupervisor::Activate_Texture(&textureNames);
		CSupervisor::Activate_Pipeline();

		object.Register((CApplication*)this);//, vertices3D, indices3D
		
		CApplication::initialize();
	}

	void update(){
		//descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		ModelPushConstants pushConstants;
		// pushConstants.model = glm::mat4(1, 0, 0, 0,
		// 							    0, 1, 0, 0,
		// 								0 ,0, 1, 0,
		// 								0, 0, 0, 1);
		pushConstants.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		renderer.PushConstantToCommand<ModelPushConstants>(pushConstants, renderProcess.graphicsPipelineLayout, shaderManager.pushConstantRange);

		object.Draw();
	}

};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif