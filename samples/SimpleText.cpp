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



		// SDL_Color white = {255, 255, 255, 255};
    	// SDL_Surface* textSurface = TTF_RenderText_Blended(sdlManager.m_font, "hello你好", 11, white); //textSurface->pixels is RGBA

		// int width  = textSurface->w;
		// int height = textSurface->h;
		// void* pixels = textSurface->pixels; //this saves the pixel data of the rendered text
		// int pitch = textSurface->pitch;

		// std::cout << "Text Surface Width: " << width << ", Height: " << height << ", Pitch: " << pitch << std::endl;






		// SDL_Renderer* renderer = SDL_CreateRenderer(sdlManager.window, NULL);
		// SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		// SDL_FRect textRect = {20, 20, textSurface->w, textSurface->h};
		// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		// SDL_RenderClear(renderer);
		// SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
		// SDL_RenderPresent(renderer);
		// TTF_CloseFont(sdlManager.m_font);
		// TTF_Quit();






		// CTextureImage textureImage;
		// textureImage.m_imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
		// textureImage.m_mipLevels = 1;
		// textureImage.m_usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		// textureImage.m_pCommandPool = &renderer.commandPool;

		// //textureImage.GetTexels(texturePath);
		// textureImage.m_pTexels = pixels;
		// textureImage.m_texWidth = width;
		// textureImage.m_texHeight = height;
		// textureImage.m_texChannels = 4;// STBI_rgb_alpha, RGBA
		// textureImage.m_texBptpc = 8;

		// textureImage.CreateTextureImage(false); //false: not use STBI to free pixels
		// SDL_DestroySurface(textSurface);
		// textureImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		// textureImage.m_sampler_id = 0;// sampler_id;

		// textureManager.textureImages.pop_back();
		// textureManager.textureImages.push_back(textureImage);



		//std::cout<<"Finished creating texture image and image view."<<std::endl;

	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		//objects[0].Draw();
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	}	
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif