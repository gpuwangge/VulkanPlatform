#include "../include/object.h"
#include "../include/application.h"

/******************
* Object
*******************/
CObject::CObject(){
    bRegistered = false;
    bUseMVP_VP = false;
    bSticker = false;
    //bUseTextureSampler = false;
    bSkybox = false;
    bUpdate = true;

    m_object_id = 0;
    //std::vector<int> m_texture_ids;
    m_model_id = 0;
    m_graphics_pipeline_id = 0;

    Length_original = glm::vec3();
    LengthMin_original = glm::vec3();
    LengthMax_original = glm::vec3();
    Length = glm::vec3();

    //ObjectState = ObjectStates::IDLE;

    Position = glm::vec3();
    Rotation = glm::vec3();
    Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    Velocity = glm::vec3();
    AngularVelocity = glm::vec3();

    for(int i = 0; i < 6; i++) TempVelocity[i] = glm::vec4();
    for(int i = 0; i < 6; i++) TempAngularVelocity[i] = glm::vec4();

    RotationMatrix = glm::mat4();
    ScaleMatrix = glm::mat4();
    DirectionFront = glm::vec3();
    DirectionUp = glm::vec3();
    DirectionLeft = glm::vec3();

    TempMoveVelocity = glm::vec4();
    TempMoveAngularVelocity = glm::vec4();
    // MaxSpeed = 1.0f;
    // Speed = 0.0f;
    // MaxRotateSpeed = 50.0f;
    // RotateSpeed = 0.0f;
    // TargetPosition = glm::vec3();
    // TargetDirection = glm::vec3();

    //Scale = 1.0f;
}

 void CObject::Update(float deltaTime){
    if(!bRegistered)  return;
    if(!bUpdate) return;
    //std::cout<<"Update Object"<<m_object_id<<std::endl;
    //std::cout<<"deltaTime: "<<deltaTime<<std::endl;

    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    //static unsigned count = 0;
    //if(count % 3000 == 0){
    //    std::cout<<"Object Position="<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;
        //std::cout<<"Object Rotation="<<Rotation.x<<","<<Rotation.y<<","<<Rotation.z<<std::endl;
    //}
    //count++;

    /**********
    * Calculate model matrix based on Translation, Rotation and Scale
    **********/
    CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = TranslateMatrix * RotationMatrix * ScaleMatrix;
 }

// float CObject::ComputeDifference(glm::vec3 v1, glm::vec3 v2){
//     return glm::sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
// }


//void CObject::SetRotation(glm::mat4 m){
    //graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//objectList[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    //CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//objectList[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    
    //CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = m;
//}


void CObject::CleanUp(){
    //textureDescriptor.DestroyAndFree();
}

// void CObject::CreateTextureDescriptorSets(CTextureImage &textureImage, VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, VkSampler &sampler, std::vector<VkImageView> *swapchainImageViews){
//     std::vector<CTextureImage> textureImages{textureImage};
//     std::vector<VkSampler> samplers{sampler};
//     CreateTextureDescriptorSets(textureImages, descriptorPool, descriptorSetLayout, samplers, swapchainImageViews);
// }

void CObject::CreateTextureDescriptorSets(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews){
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

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets.data());
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
            if(j < m_texture_ids.size()){
                imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
                imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
                imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
                imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            }
            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = descriptorSets[i];
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

void CObject::Register(CApplication *p_app, int object_id, std::vector<int> texture_ids, int model_id, int graphics_pipeline_id){
    m_object_id = object_id; 
    m_texture_ids = texture_ids; 
    m_model_id = model_id; 
    m_graphics_pipeline_id = graphics_pipeline_id; 
    bUseMVP_VP = CGraphicsDescriptorManager::CheckMVP();

    if(CSupervisor::VertexStructureType == VertexStructureTypes::TwoDimension || CSupervisor::VertexStructureType  == VertexStructureTypes::ThreeDimension){
        Length_original = p_app->modelManager.modelLengths[model_id];
        LengthMin_original = p_app->modelManager.modelLengthsMin[model_id];
        LengthMax_original = p_app->modelManager.modelLengthsMax[model_id];
    }else{
        Length_original = glm::vec3();
        LengthMin_original = glm::vec3();
        LengthMax_original = glm::vec3();        
    }
    Length = Length_original;
    //std::cout<<"Length = "<<Length.x<<", "<<Length.y<<", "<<Length.z<<std::endl;
    //std::cout<<"LengthMin = "<<LengthMin.x<<", "<<LengthMin.y<<", "<<LengthMin.z<<std::endl;
    //std::cout<<"LengthMax = "<<LengthMax.x<<", "<<LengthMax.y<<", "<<LengthMax.z<<std::endl;
    
    //Prepare pointers for drawcall
    p_renderer = &(p_app->renderer);
    p_renderProcess = &(p_app->renderProcess);
    p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayouts[m_graphics_pipeline_id]);
    p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);
    p_textureManager = &(p_app->textureManager);


    //there are up to 3 samplers, support up to 3 different textures
    //for(int i = 0; i < m_texture_ids.size(); i++)
    //    std::cout<<"Register object:"<<m_object_id<<" with texture:"<<m_texture_ids[i]<<" and model:"<<model_id<<" and graphcis pipeline:"<<m_graphics_pipeline_id<<std::endl;

    //bUseTextureSampler = true; 
    if(m_texture_ids.size() > 0){
        CreateTextureDescriptorSets(
            CGraphicsDescriptorManager::descriptorPool,
            CGraphicsDescriptorManager::textureDescriptorSetLayout,
            CGraphicsDescriptorManager::textureSamplers
        );
    }


    //Create texture descriptor set
    // if(m_texture_ids[i] != -1){
    //     bUseTextureSampler = true; 
    //     //if(texture_id != INT_MAX){
    //         //bUseCubemap = p_app->textureManager.textureImages[m_texture_id].bEnableCubemap; //if this object is skybox, the view matrix should ignore translate
        
    //     CreateTextureDescriptorSets(
    //         p_app->textureManager.textureImages[m_texture_id], 
    //         CGraphicsDescriptorManager::descriptorPool,
    //         CGraphicsDescriptorManager::textureDescriptorSetLayout,
    //         CGraphicsDescriptorManager::textureSamplers[0]
    //     );
    //     // }else{
    //     //     //bUseCubemap = false; //the skybox with cubemap should use only one texture
    //     //     CreateTextureDescriptorSets(
    //     //         p_app->textureManager.textureImages, 
    //     //         CGraphicsDescriptorManager::descriptorPool,
    //     //         CGraphicsDescriptorManager::textureDescriptorSetLayout,
    //     //         CGraphicsDescriptorManager::textureSamplers
    //     //     );
    //     // }
    // }
    bRegistered = true;
}


void CObject::Draw(uint32_t n){
    if(!bRegistered) return;
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[m_graphics_pipeline_id], 
        VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);
    //std::cout<<"test2. p_graphicsDescriptorSets->size()="<<p_graphicsDescriptorSets->size()<<std::endl;
    //std::cout<<"test2. m_texture_ids.size()="<<m_texture_ids.size()<<std::endl;

    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    //set = 0 is for general uniform; set = 1 is for texture sampler uniform
    if(CGraphicsDescriptorManager::getSetSize() > 0) dsSets.push_back(*p_graphicsDescriptorSets); 
    if(CGraphicsDescriptorManager::textureSamplers.size() > 0) dsSets.push_back(descriptorSets); 
    //std::cout<<"test3.dsSets.size()="<<dsSets.size()<<std::endl;
    
    if(dsSets.size() > 0){
        int dynamicOffsetIndex = -1; //-1 means not use dynamic offset (no MVP/VP used)
        if(bUseMVP_VP) dynamicOffsetIndex = m_object_id; //assume descriptor uniform(MVP/VP) offset is m_id
        p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, dynamicOffsetIndex);
    }//else std::cout<<"No Descritpor is used."<<std::endl;
    //std::cout<<"test4."<<std::endl;
    //if(!vertices3D.empty() || !vertices2D.empty()){
    p_renderer->BindVertexBuffer(m_model_id);
    //}//else std::cout<<"No vertex buffer is used."<<std::endl;
    //std::cout<<"test5."<<std::endl;
    //if(indices3D.empty()){
    if(p_renderer->indices3Ds.empty()){
        //std::cout<<"No index buffer is used."<<std::endl;
        p_renderer->Draw(n);
    }else{
        p_renderer->BindIndexBuffer(m_model_id);
        p_renderer->DrawIndexed(m_model_id);
    }
   //std::cout<<"test6."<<std::endl;
}


void CObject::Draw(std::vector<CWxjBuffer> &buffer, uint32_t n){ //const VkBuffer *pBuffers
    //this function is used in sample:simpleparticle only
    //std::cout<<"testdraw1,"<<m_graphics_pipeline_id<<","<<p_renderer->graphicsCmdId<<std::endl;
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[m_graphics_pipeline_id], 
    //p_renderer->BindPipeline(p_renderProcess->graphicsPipeline, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //TODO: bind descriptor set
    //std::cout<<"testdraw2"<<std::endl;

    //VkDeviceSize offsets[] = { 0 };
    //vkCmdBindVertexBuffers(p_renderer->commandBuffers[p_renderer->graphicsCmdId][p_renderer->currentFrame], 0, 1, &buffer[p_renderer->currentFrame].buffer, offsets);
    p_renderer->BindExternalBuffer(buffer);
    p_renderer->Draw(n);
    //std::cout<<"testdraw3"<<std::endl;
}
