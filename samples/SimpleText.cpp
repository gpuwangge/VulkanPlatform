#include "..\\vulkanFramework\\include\\application.h"
//#include <SDL3_ttf/SDL_ttf.h>

// #ifdef _WIN32
// #include <windows.h>
#define TEST_CLASS_NAME CSimpleText
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	void initialize(){
		modelManager.CreateCustomModel3D(vertices3D, indices3D); //model for background
		CApplication::initialize();
	}

	void update(){
		// static int lastTrigger = 0;
		// int currentTrigger = (double)elapseTime / 0.1f;
		// if(currentTrigger!=lastTrigger) {
		// 	lastTrigger = currentTrigger;
		// }
		

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		//objects[0].Draw();
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
		textManager.Draw();
	}	
};

//#endif

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif