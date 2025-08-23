#include "..\\vulkanFramework\\include\\application.h"
#include <SDL3_ttf/SDL_ttf.h>
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
		modelManager.CreateCustomModel3D(vertices3D, indices3D);
		CApplication::initialize();



		SDL_Color white = {255, 255, 255, 255};
    	SDL_Surface* textSurface = TTF_RenderText_Blended(sdlManager.m_font, "hello你好", 11, white); //textSurface->pixels is RGBA

		int width  = textSurface->w;
		int height = textSurface->h;
		void* pixels = textSurface->pixels; //this saves the pixel data of the rendered text
		int pitch = textSurface->pitch;

		std::cout << "Text Surface Width: " << width << ", Height: " << height << ", Pitch: " << pitch << std::endl;

		// SDL_Renderer* renderer = SDL_CreateRenderer(sdlManager.window, NULL);
		// SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		// SDL_FRect textRect = {20, 20, textSurface->w, textSurface->h};
		// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		// SDL_RenderClear(renderer);
		// SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
		// SDL_RenderPresent(renderer);
		// TTF_CloseFont(sdlManager.m_font);
		// TTF_Quit();

	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		objects[0].Draw();
	}	
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif