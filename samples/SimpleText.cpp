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
		static int lastTrigger = 0;
		int currentTrigger = (double)elapseTime / 0.1f;
		if(currentTrigger!=lastTrigger) {
			textManager.m_textBoxes[0].SetTextContent("FPS:" + std::to_string((int)(1.0f/deltaTime)));
			textManager.m_textBoxes[1].SetTextContent("Time:" + to_string_prec(elapseTime) + "s");
			textManager.m_textBoxes[2].SetTextContent("Frame:" + std::to_string(frameCount));
			textManager.m_textBoxes[3].SetTextContent("Init:" + to_string_prec(totalInitTime) + " ms");
			textManager.m_textBoxes[4].SetTextContent("Object:" + std::to_string(objects.size()));
			textManager.m_textBoxes[5].SetTextContent("Textbox:" + std::to_string(textManager.m_textBoxes.size()));
			textManager.m_textBoxes[6].SetTextContent("Light:" + std::to_string(lights.size()));
			std::string r = "false";
			if(renderProcess.iShadowmapAttachmentDepthLight>=0) r = "true"; else r= "false";
			textManager.m_textBoxes[9].SetTextContent("Depth(Light):" + r);
			if(renderProcess.iMainSceneAttachmentDepthLight>=0) r = "true"; else r= "false";
			textManager.m_textBoxes[11].SetTextContent("Depth(Light):" + r);
			if(renderProcess.iMainSceneAttachmentDepthCamera>=0) r = "true"; else r= "false";
			textManager.m_textBoxes[12].SetTextContent("Depth(Camera):" + r);
			if(renderProcess.iMainSceneAttachmentColorResovle>=0) r = "true"; else r= "false";
			textManager.m_textBoxes[13].SetTextContent("Color(Resolve):" + r);
			if(renderProcess.iMainSceneAttachmentColorPresent>=0) r = "true"; else r= "false";
			textManager.m_textBoxes[14].SetTextContent("Color(Present):" + r);
			//for(int i = 0; i < textManager.m_textBoxes.size(); i++) textManager.m_textBoxes[i].IncHighlightedChar();
			lastTrigger = currentTrigger;
		}
		

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