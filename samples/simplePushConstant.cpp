#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePushConstant
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	void initialize(){
		modelManager.CreateCustomModel3D(vertices3D, indices3D);
		mainCamera.SetPosition(0.0f, 0.0f, -1.5f);
		appInfo.Object.Count = 1;
		appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"CUSTOM3D0"});
		appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"texture.jpg", false}});
		appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Shader.Vertex = "simplePushConstant/vert.spv";
		appInfo.Shader.Fragment = "simplePushConstant/frag.spv";
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsVP = true;
		appInfo.Feature.EnableGraphicsPushConstant = true;
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

		objectList[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif