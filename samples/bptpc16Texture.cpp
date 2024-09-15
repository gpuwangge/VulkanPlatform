#include "..\\framework\\include\\application.h"
#include "object.h"
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

	CObject object;

	std::vector<std::pair<std::string, bool>> textureNames = {{"48bpt.png", true}}; //first: textureName, second: mipmap
	std::string vertexShader = "bptpc16Texture/vert.spv";
	std::string fragmentShader = "bptpc16Texture/frag.spv";

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
		mainCamera.setPerspective(90.0f, (float)windowWidth /  (float)windowHeight, 0.1f, 256.0f);

		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		Activate_Uniform_Graphics_Sampler();
		Activate_Uniform_Graphics_MVP();
		Activate_Feature_Graphics_MSAA();
		Activate_Feature_Graphics_48BPT();
		Activate_Buffer_Graphics_Vertex(vertices3D, indices3D);
		Activate_Texture(&textureNames);
		Activate_Pipeline();

		object.Register((CApplication*)this); //, vertices3D, indices3D

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}

};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif