#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CBptpc16Texture
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	void initialize(){
		modelManager.CreateCustomModel3D(vertices3D, indices3D);
		mainCamera.SetPosition(0.0f, 0.0f, -3.0f);
		mainCamera.setPerspective(90.0f, (float)windowWidth /  (float)windowHeight, 0.1f, 256.0f);
		//appInfo.Object.Count = 1;
		//appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"CUSTOM3D0"});
		//appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		//appInfo.Object.Texture.Names = std::make_unique<std::vector<std::pair<std::string, bool>>>(std::vector<std::pair<std::string, bool>> {{"48bpt.png", true}});
		//appInfo.Object.Texture.List = std::make_unique<std::vector<int>>(std::vector<int> {0});

		//appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"bptpc16Texture/vert.spv"});
		//appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {"bptpc16Texture/frag.spv"});
		//appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string> {});
		//appInfo.Object.Pipeline.List = std::make_unique<std::vector<int>>(std::vector<int> {0});
		appInfo.Uniform.GraphicsSampler.Count = 1;
		appInfo.Uniform.EnableGraphicsMVP = true;
		appInfo.Feature.EnableGraphicsMSAA = true;
		appInfo.Feature.EnableGraphics48BPT = true;
		CApplication::initialize();
	}

	void update(){
		objectList[0].SetAngularVelocity(0,0,50.0f); //clockwise rotation around z axis
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw();
	}

};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif