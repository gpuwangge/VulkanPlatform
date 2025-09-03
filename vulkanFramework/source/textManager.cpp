#include "../include/textManager.h"
#include "../include/application.h"

/*************
* Change from CCharacter(no use) to CTextBox
**************/

void CTextBox::CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews){
    //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;
    if(samplers.size() < 1) return;
    
    int descriptorSize = samplers.size();//getDescriptorSize();
    //std::cout<<"createTextureDescriptorSets::samplers.size(): "<<samplers.size()<<std::endl;
    //std::cout<<"Object Set(Sampler) size = "<<descriptorSize<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_graphics_texture_image_sampler.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets_graphics_texture_image_sampler.data());
    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        std::vector<VkWriteDescriptorSet> descriptorWrites;

        descriptorWrites.resize(descriptorSize);
        //int counter = 0;

        VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform

        std::vector<VkDescriptorImageInfo> imageInfo{}; //for texture sampler
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT){
        imageInfo.resize(samplers.size());
        for(int j = 0; j < samplers.size(); j++){
            //std::cout<<"CreateTextureDescriptorSets::samplers:"<<j<<std::endl;
            imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_GENERAL; //test compute storage image: ?need figure this out. VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            //imageInfo[j].imageView = textureImages[j].m_textureImageBuffer.view;
            //imageInfo[j].sampler = samplers[j];

            //if(b_isText){
                //if(j < m_text_ids.size()){
                    imageInfo[j].imageView = p_textImageManager->textureImages[0].m_textureImageBuffer.view; //hack, m_text_ids[j]
                    imageInfo[j].sampler = samplers[p_textImageManager->textureImages[0].m_sampler_id]; 
                //}else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
                //    imageInfo[j].imageView = p_textImageManager->textureImages[m_text_ids[0]].m_textureImageBuffer.view;
                //    imageInfo[j].sampler = samplers[p_textImageManager->textureImages[m_text_ids[0]].m_sampler_id]; 
                //}
            // }else{
            //     if(j < m_texture_ids.size()){
            //         imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
            //         imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            //     }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
            //         imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
            //         imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            //     }
            // }


            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = descriptorSets_graphics_texture_image_sampler[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pImageInfo = &imageInfo[j];
            //counter++;
        }
        //}

        //Step 4
        vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    //std::cout<<"Done set descriptor. "<<std::endl;
}

void CTextBox::Draw(){
    //std::cout<<"Draw character"<<std::endl;

    int current_graphics_pipeline_id = 2;//hack, (graphicsPipelineId == -1) ? m_default_graphics_pipeline_id : graphicsPipelineId;

    VkPipelineLayout *p_graphicsPipelineLayout = &(p_renderProcess->graphicsPipelineLayouts[current_graphics_pipeline_id]);
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[current_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    //set = 0 is for general uniform; set = 1 is for texture sampler uniform
    if(CGraphicsDescriptorManager::getSetSize_General() > 0) dsSets.push_back(*p_descriptorSets_graphics_general); 
    if(CGraphicsDescriptorManager::textureImageSamplers.size() > 0) dsSets.push_back(descriptorSets_graphics_texture_image_sampler); 

    //bool bUseMVP_VP = true; //hack
    //int m_object_id = 2;//hack
    if(dsSets.size() > 0){
        //int dynamicObjectMVPOffset = -1; //-1 means not use dynamic offset (no MVP/VP used)
        int dynamicTextboxMVPOffset = m_textBoxID; //use offset for textboxID=0
        p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, 0, dynamicTextboxMVPOffset);
    }

    int m_model_id = 2;//hack
    // p_renderer->BindVertexBuffer(m_model_id);
    // p_renderer->BindIndexBuffer(m_model_id);
    // p_renderer->DrawIndexed(m_model_id);
    //p_renderer->BindVertexBuffer(m_model_id);
    p_renderer->BindVertexInstanceBuffer(m_model_id);
    p_renderer->BindIndexBuffer(m_model_id);
    p_renderer->DrawInstanceIndexed(m_model_id, m_instanceCount);

}


/******************
* TextBox
*******************/
//CTextBox::CTextBox(){}
//CApplication *p_app, int object_id, std::vector<int> texture_ids, std::vector<int> text_ids, int model_id, int default_graphics_pipeline_id
void CTextBox::Register(CApplication *p_app, int textbox_id, std::vector<int> text_ids, std::string content){
    bRegistered = true;
    m_textBoxID = textbox_id;

    //m_characters.resize(1);//TODO: change later

    m_instanceCount = p_app->textManager.GetInstanceCount();

    //for(auto& ch : m_characters){
    //ch.SetInstanceCount(m_instanceCount);
    p_renderer = &(p_app->renderer);
    p_renderProcess = &(p_app->renderProcess);
    p_descriptorSets_graphics_general = &(p_app->graphicsDescriptorManager.descriptorSets_general);
    //ch.descriptorSets_graphics_texture_image_sampler;
    p_textImageManager = &(p_app->textImageManager);
    CreateDescriptorSets_TextureImageSampler(
        CGraphicsDescriptorManager::graphicsDescriptorPool,
        CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler,
        CGraphicsDescriptorManager::textureImageSamplers
    );
    //}
}

void CTextBox::Update(float deltaTime, int currentFrame, Camera &mainCamera){
    if(!bRegistered) return;
    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix
    /********************************
    * Calculate model matrix based on Translation, Rotation and Scale
    ********************************/
   if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP){
        CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].model = TranslateMatrix * RotationMatrix * ScaleMatrix;

        //std::cout<< "TextBox ID: " << m_textBoxID << " Model Matrix: " << glm::to_string(CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].model) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TranslateMatrix: " << glm::to_string(TranslateMatrix) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " RotateMatrix: " << glm::to_string(RotationMatrix) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " ScaleMatrix: " << glm::to_string(ScaleMatrix) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " Velocity: " << glm::to_string(Velocity) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " AngularVelocity: " << glm::to_string(AngularVelocity) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TempVelocity: " << glm::to_string(TempVelocity[0]) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TempAngularVelocity: " << glm::to_string(TempAngularVelocity[0]) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TempMoveAngularVelocity: " << glm::to_string(TempMoveAngularVelocity) << std::endl;
        //std::cout<< "Delta Time: " << deltaTime << std::endl;

        CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].mainCameraProj = mainCamera.matrices.projection;
        CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].mainCameraView = mainCamera.matrices.view;
        memcpy(CGraphicsDescriptorManager::textMVPUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::textMVPUBO, sizeof(CGraphicsDescriptorManager::textMVPUBO));
   }
    //for(auto& ch : m_characters){ 
        //ch.Update();
    //}
}

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

/*There are two resources: quad pos/uv and instance offset/color*/
void CTextManager::CreateTextResource(){
    
    int w = 51;
    int h = 73;
    float aspect = (float)w / (float)h; // 31/73 ≈ 0.4247

    float half_h = 0.15f;         // height fixed to 1.0（y=-0.5~0.5）
    float half_w = half_h * aspect; // width scaled proportionally

    float x_min = -half_w; //-0.5f;
    float x_max =  half_w; //0.5f;
    float y_min = -half_h; //-0.5f;
    float y_max =  half_h; //0.5f;

    char ch = 'w';
    GlyphTexture &g = glyphMap[ch];
    float v0 = g.v0;
    float v1 = g.v1;
    float u0 = g.u0;
    float u1 = g.u1;

    std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

    std::vector<Vertex3D> textQuadVertices;
    textQuadVertices.push_back({ {x_min, y_min, 0.0f}, {1,0,0}, {u1, v0} }); // left bottom
    textQuadVertices.push_back({ {x_max, y_min, 0.0f}, {1,0,0}, {u0, v0} }); // right bottom
    textQuadVertices.push_back({ {x_max, y_max, 0.0f}, {1,0,0}, {u0, v1} }); // right up
    textQuadVertices.push_back({ {x_min, y_max, 0.0f}, {1,0,0}, {u1, v1} }); // left up
    p_modelManager->CreateCustomModel3D(textQuadVertices, indices3D); //model for text


    std::vector<TextQuadVertex> textQuadVertices2D;
    textQuadVertices2D.push_back({ {x_min, y_min}, {u1, v1} }); // left bottom
    textQuadVertices2D.push_back({ {x_max, y_min}, {u0, v1} }); // right bottom
    textQuadVertices2D.push_back({ {x_max, y_max}, {u0, v0} }); // right up
    textQuadVertices2D.push_back({ {x_min, y_max}, {u1, v0} }); // left up

    std::vector<TextInstanceData> textInstanceData;
    textInstanceData.push_back({ {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }); // red
    textInstanceData.push_back({ {0.2f, 0.0f}, {0.0f, 1.0f, 0.0f} }); // green
    textInstanceData.push_back({ {0.4f, 0.0f}, {0.0f, 0.0f, 1.0f} }); // blue
    textInstanceData.push_back({ {0.6f, 0.0f}, {1.0f, 0.0f, 1.0f} });

    p_modelManager->CreateTextModel(textQuadVertices2D, textInstanceData, indices3D);
    std::cout<<"text model created in modelManager.textModel, size = "<<p_modelManager->textModels.size()<<std::endl;


    m_instanceCount = textInstanceData.size();
    //std::cout<<"text quad model created"<<std::endl;
}

// void CTextManager::AddTextBox(const CTextBox& textBox) {
//     m_textBoxes.push_back(textBox);
// }

void CTextManager::Update(float deltaTime, int currentFrame, Camera &mainCamera){
    for (int i = 0; i < m_textBoxes.size(); i++) {
        m_textBoxes[i].Update(deltaTime, currentFrame, mainCamera);
    }
}

// void CTextManager::Draw(){
//      for (int i = 0; i < m_textBoxes.size(); i++) {
//         m_textBoxes[i].Draw();
//      }
// }




