#include "../include/object.h"
#include "../include/application.h"

/******************
* Object
*******************/
CObject::CObject(){
    bUseMVP_VP = false;
    bSticker = false;
    bUseTextureSampler = false;
    bUpdate = true;

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

void CObject::CreateTextureDescriptorSets(CTextureImage &textureImage, VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, VkSampler &sampler, std::vector<VkImageView> *swapchainImageViews){
    std::vector<CTextureImage> textureImages{textureImage};
    std::vector<VkSampler> samplers{sampler};
    CreateTextureDescriptorSets(textureImages, descriptorPool, descriptorSetLayout, samplers, swapchainImageViews);
}

void CObject::CreateTextureDescriptorSets(std::vector<CTextureImage> &textureImages, VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews){
    //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;
    
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
            imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_GENERAL; //test compute storage image: ?need figure this out. VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            imageInfo[j].imageView = textureImages[j].m_textureImageBuffer.view;
            imageInfo[j].sampler = samplers[j];
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

void CObject::Register(CApplication *p_app, int texture_id, int model_id, int object_id, glm::vec3 length, glm::vec3 lengthMin, glm::vec3 lengthMax){ //-1 means no texture or model 
    m_object_id = object_id;
    m_texture_id = texture_id;
    m_model_id = model_id;
    bUseMVP_VP = CGraphicsDescriptorManager::CheckMVP();

    Length_original = length;
    LengthMin_original = lengthMin;
    LengthMax_original = lengthMax;
    Length = length;
    //std::cout<<"Length = "<<Length.x<<", "<<Length.y<<", "<<Length.z<<std::endl;
    //std::cout<<"LengthMin = "<<LengthMin.x<<", "<<LengthMin.y<<", "<<LengthMin.z<<std::endl;
    //std::cout<<"LengthMax = "<<LengthMax.x<<", "<<LengthMax.y<<", "<<LengthMax.z<<std::endl;
    
    //Prepare pointers for drawcall
    p_renderer = &(p_app->renderer);
    p_renderProcess = &(p_app->renderProcess);
    p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
    p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

    //Create texture descriptor set
    if(texture_id != -1){
        bUseTextureSampler = true; //vertex3D has texture coord, assume texture is used
        if(texture_id != INT_MAX){
            CreateTextureDescriptorSets(
                p_app->textureManager.textureImages[m_texture_id], 
                CGraphicsDescriptorManager::descriptorPool,
                CGraphicsDescriptorManager::textureDescriptorSetLayout,
                CGraphicsDescriptorManager::textureSamplers[0]
            );
        }else{
            CreateTextureDescriptorSets(
                p_app->textureManager.textureImages, 
                CGraphicsDescriptorManager::descriptorPool,
                CGraphicsDescriptorManager::textureDescriptorSetLayout,
                CGraphicsDescriptorManager::textureSamplers
            );
        }
    }

}


void CObject::Draw(uint32_t n){
    p_renderer->BindPipeline(p_renderProcess->graphicsPipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    if(p_graphicsDescriptorSets->size() > 0) dsSets.push_back(*p_graphicsDescriptorSets); //set = 0, general uniform
    if(bUseTextureSampler) dsSets.push_back(descriptorSets); //set = 1, texture sampler uniform

    if(dsSets.size() > 0){
        int dynamicOffsetIndex = -1; //-1 means not use dynamic offset (no MVP/VP used)
        if(bUseMVP_VP) dynamicOffsetIndex = m_object_id; //assume descriptor uniform(MVP/VP) offset is m_id
        p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, dynamicOffsetIndex);
    }//else std::cout<<"No Descritpor is used."<<std::endl;

    //if(!vertices3D.empty() || !vertices2D.empty()){
    if(m_model_id!=-1) p_renderer->BindVertexBuffer(m_model_id);
    //}//else std::cout<<"No vertex buffer is used."<<std::endl;
    
    //if(indices3D.empty()){
    if(p_renderer->indices3Ds.empty()){
        //std::cout<<"No index buffer is used."<<std::endl;
        p_renderer->Draw(n);
    }else{
        p_renderer->BindIndexBuffer(m_model_id);
        p_renderer->DrawIndexed(m_model_id);
    }
}


void CObject::Draw(std::vector<CWxjBuffer> &buffer, uint32_t n){ //const VkBuffer *pBuffers
    //this function is used in sample:simpleparticle only

    p_renderer->BindPipeline(p_renderProcess->graphicsPipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //TODO: bind descriptor set

    //VkDeviceSize offsets[] = { 0 };
    //vkCmdBindVertexBuffers(p_renderer->commandBuffers[p_renderer->graphicsCmdId][p_renderer->currentFrame], 0, 1, &buffer[p_renderer->currentFrame].buffer, offsets);
    p_renderer->BindExternalBuffer(buffer);
    p_renderer->Draw(n);
}
