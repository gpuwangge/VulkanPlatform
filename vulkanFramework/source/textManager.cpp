#include "../include/textManager.h"
#include "../include/application.h"

/******************
* TextBox
*******************/
CTextBox::CTextBox(){}


/******************
* TextManager
*******************/
CTextManager::CTextManager(){
}

void CTextManager::CreateTextFonts(){
    if (TTF_Init() == -1) std::cout << "SDL_ttf could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    m_font = TTF_OpenFont("NotoSansCJK-VF.otf.ttc", m_fontSize);
    if (!m_font) std::cout << "Failed to load font! SDL_Error: " << SDL_GetError() << std::endl;

    TTF_SetFontStyle(m_font, TTF_STYLE_BOLD);
}

void CTextManager::CreateTextImage(){
    //Step 1: Create Text Resources
    CreateTextFonts();

    // SDL_Color sdlColor = { 
    //     static_cast<Uint8>(m_color.size() > 0 ? color[0] : 255),
    //     static_cast<Uint8>(color.size() > 1 ? color[1] : 255),
    //     static_cast<Uint8>(color.size() > 2 ? color[2] : 255),
    //     static_cast<Uint8>(color.size() > 3 ? color[3] : 255)
    // };
    SDL_Color sdlColor = { 
        static_cast<Uint8>(m_color.r),
        static_cast<Uint8>(m_color.g),
        static_cast<Uint8>(m_color.b),
        static_cast<Uint8>(m_color.a)
    };
    //SDL_Surface* textSurface = TTF_RenderText_Blended(sdlManager.m_font, "1234567890hello你好ABC王小军", 0, sdlColor); //textSurface->pixels is RGBA
    SDL_Surface* textSurface = TTF_RenderText_Blended(m_font, ascII.c_str(), 0, sdlColor);

    SDL_Surface* conv = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_RGBA32);
    if (!conv) {
        std::cerr << "SDL_ConvertSurfaceFormat failed: " << SDL_GetError() << std::endl;
    }
    
    int width  = conv->w;
    int height = conv->h;
    void* pixels = conv->pixels; //this saves the pixel data of the rendered text
    int pitch = conv->pitch;

    //std::cout << "Text Surface '" << 123 << "' Width: " << width << ", Height: " << height << ", Pitch: " << pitch << std::endl;

    CTextureImage textureImage;
    textureImage.m_imageFormat = VK_FORMAT_R8G8B8A8_UNORM;//VK_FORMAT_R8G8B8A8_SRGB;
    textureImage.m_mipLevels = 1;
    textureImage.m_usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    textureImage.m_pCommandPool = &(p_renderer->commandPool);

    //textureImage.GetTexels(texturePath);
    textureImage.m_pTexels = pixels;
    textureImage.m_texWidth = width;
    textureImage.m_texHeight = height;
    textureImage.m_texChannels = 4;// STBI_rgb_alpha, RGBA
    textureImage.m_texBptpc = 8;

    textureImage.CreateTextureImage(false); //false: not use STBI to free pixels
    SDL_DestroySurface(conv);
    SDL_DestroySurface(textSurface);
    textureImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

    textureImage.m_sampler_id = m_samplerID;

    //std::cout<<"Text Image Created: "<<std::endl;

    p_textImageManager->textureImages.push_back(textureImage);
}

void CTextManager::CreateGlyphMap(){
//int minx, maxx, miny, maxy, advance; //not used here
    SDL_Surface* glyphSurface;
    GlyphTexture glyph;
    int penX = 0; int penY = 0;
    int maxRowHeight = 0;
    for(int i = 0; i < ascII.size(); i++){
        char ch = ascII[i];
        //TTF_GetGlyphMetrics(sdlManager.m_font, ch, &minx, &maxx, &miny, &maxy, &advance);
        glyphSurface = TTF_RenderGlyph_Blended(m_font, ch, {255, 255, 255, 255});
        if (!glyphSurface) continue;

        glyph.rect = {penX, penY, glyphSurface->w, glyphSurface->h}; // Assuming fixed width for simplicity
        //glyph.advance = advance;
        glyphMap[ch] = glyph;

        penX += glyphSurface->w + 1;
        if (glyphSurface->h > maxRowHeight) maxRowHeight = glyphSurface->h;

        SDL_DestroySurface(glyphSurface);
    }
    int atlasWidth = glyphMap[ascII.back()].rect.x + glyphMap[ascII.back()].rect.w;
    int atlasHeight = maxRowHeight;

    for(int i = 0; i < ascII.size(); i++){		
        char ch = ascII[i];
        glyph = glyphMap[ch];
        // Calculate normalized UV coordinates
        glyphMap[ch].u0 = (float)glyph.rect.x / atlasWidth;
        glyphMap[ch].v0 = (float)glyph.rect.y / atlasHeight;
        glyphMap[ch].u1 = (float)(glyph.rect.x + glyph.rect.w) / atlasWidth;
        glyphMap[ch].v1 = (float)(glyph.rect.y + glyph.rect.h) / atlasHeight;
        // std::cout << "Normalized UVs for character '" << ch << "': u0=" << u0 << "; u1=" << u1 << ";" << std::endl;
    }
}

void CTextManager::CreateTextModel(){
    std::vector<Vertex3D> textQuadVertices;
    int w = 51;
    int h = 73;
    float aspect = (float)w / (float)h; // 31/73 ≈ 0.4247

    float half_h = 0.15f;         // 高度固定 1.0（y=-0.5~0.5）
    float half_w = half_h * aspect; // 宽度按比例缩放

    float x_min = -half_w;
    float x_max =  half_w;
    float y_min = -half_h;
    float y_max =  half_h;
    // float x_min = -0.5f;
    // float x_max =  0.5f;
    // float y_min = -0.5f;
    // float y_max =  0.5f;

    
    char ch = 'w';
    GlyphTexture &g = glyphMap[ch];
    float v0 = g.v0;
    float v1 = g.v1;
    float u0 = g.u0;
    float u1 = g.u1;

    textQuadVertices.push_back({ {x_min, y_min, 0.0f}, {1,0,0}, {u1, v0} }); // left bottom
    textQuadVertices.push_back({ {x_max, y_min, 0.0f}, {1,0,0}, {u0, v0} }); // right bottom
    textQuadVertices.push_back({ {x_max, y_max, 0.0f}, {1,0,0}, {u0, v1} }); // right up
    textQuadVertices.push_back({ {x_min, y_max, 0.0f}, {1,0,0}, {u1, v1} }); // left up

    std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
    p_modelManager->CreateCustomModel3D(textQuadVertices, indices3D); //model for text

    std::cout<<"text quad model created"<<std::endl;
}

void CTextManager::AddTextBox(const CTextBox& textBox) {
    m_textBoxes.push_back(textBox);
}




