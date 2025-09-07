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

	// struct GlyphInfo {
	// 	float u0, v0, u1, v1; // UV 
	// 	int width, height;     // size
	// 	int bearingX, bearingY;// baseline
	// 	int advance;           // cursor advance for next
	// };
	// std::unordered_map<char32_t, GlyphInfo> glyphTable;


	void initialize(){
		modelManager.CreateCustomModel3D(vertices3D, indices3D); //model for background
		CApplication::initialize();
	}

	void update(){
		/*
		static int count = 0;
		static int charIndex = 0;
		count++;
		char ch = textManager.ascII[charIndex % textManager.ascII.size()];
		GlyphTexture &g = textManager.glyphMap[ch];
		float v0 = g.v0;
		float v1 = g.v1;
		float u0 = g.u0;
		float u1 = g.u1;

		//change model manager's model data doesn't work
		modelManager.customModels3D[1].vertices[0].texCoord = { u1, v0 };// left bottom
		modelManager.customModels3D[1].vertices[1].texCoord = { u0, v0 };// right bottom
		modelManager.customModels3D[1].vertices[2].texCoord = { u0, v1 };// right top
		modelManager.customModels3D[1].vertices[3].texCoord = { u1, v1 };// left top

		renderer.vertexDataBuffers[1].fill(modelManager.customModels3D[1].vertices.data());

		if(count % 300 == 0) charIndex++;
		*/

		float penX = 0.0f;
    	float penY = 0.0f;
		int rowCharCount = 0;
		//std::string text_content = "oat sx = 2.0f / WINDOW_WIDTH; Hello, World!";
		//std::string text_content = "llllllllllllllllllllllllllll";
		std::string text_content = "llllllllwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwlllllll";
		for (int i = 0; i < text_content.size(); i++) {
			GlyphTexture &glyph = textManager.glyphMap[text_content[i]];

			float sx = 2.0f / WINDOW_WIDTH; // scale to NDC
			float sy = 2.0f / WINDOW_HEIGHT;
			glm::vec2 offset(penX * sx, penY * sy);
			glm::vec2 scale(glyph.size.x / glyph.size.y, 1.0f);
			glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);

			// textManager.m_textBoxes[0].textInstanceData[0].uvRect = glyph.uvRect;
			// textManager.m_textBoxes[0].textInstanceData[0].scale = scale;
			// textManager.m_textBoxes[0].textInstanceData[0].color = color;

			modelManager.textModels[0].instanceData[i].offset = offset;
			modelManager.textModels[0].instanceData[i].uvRect = glyph.uvRect;
			modelManager.textModels[0].instanceData[i].scale = scale;
			modelManager.textModels[0].instanceData[i].color = color;

			penX += glyph.advance;
			rowCharCount++;
			if (rowCharCount >= textManager.m_textBoxes[0].m_maxCharPerRow) { //move to next line
				penX = 0;
				penY -= glyph.size.y;
				rowCharCount = 0;
			}
		}

		renderer.instanceDataBuffers[0].fill(modelManager.textModels[0].instanceData.data());

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