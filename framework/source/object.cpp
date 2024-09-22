#include "../include/object.h"
#include "../include/application.h"

CObject::CObject(){
    bUseMVP_VP = false;
    bUseTextureSampler = false;
    bUpdate = true;

    PositionVector = glm::vec3();
    Velocity = glm::vec3();
    AngularVelocity = glm::vec3();
    //MovementSpeed = 0;
    //RotationSpeed = 0;
    //Accerlation = glm::vec3();
    //Direction = glm::vec3(1,0,0);
    RotationVector = glm::vec3();
    Pitch = 0;
    Yaw = 0;
    Roll = 0;
    float Scale = 1.0f;
    //m_model = glm::mat4(1.0f);
}

 void CObject::Update(float deltaTime){
    if(!bUpdate) return;
    //std::cout<<"Update Object"<<m_object_id<<std::endl;
    
    // glm::vec3 objectUp;
    // objectUp.x = -cos(glm::radians(RotationVector.x)) * sin(glm::radians(RotationVector.y));
    // objectUp.y = sin(glm::radians(RotationVector.x));
    // objectUp.z = cos(glm::radians(RotationVector.x)) * cos(glm::radians(RotationVector.y));
    // objectUp = glm::normalize(objectUp);

    /**********
    * Vulkan Screen Coordinate: Right Hand Rule, x from left to right, y from up to bottom, z from far to near 
    **********/
    //PositionVector += deltaTime * glm::normalize(glm::cross(objectUp, glm::vec3(0.0f, 1.0f, 0.0f))) * Velocity.x;
    //PositionVector += deltaTime * glm::normalize(glm::cross(objectUp, glm::vec3(1.0f, 0.0f, 0.0f))) * Velocity.y;
    //PositionVector += deltaTime * glm::normalize(objectUp) * Velocity.z;

    ///RotationVector += deltaTime * glm::normalize(glm::cross(objectUp, glm::vec3(0.0f, 1.0f, 0.0f))) * AngularVelocity.x;
    ///RotationVector += deltaTime * glm::normalize(glm::cross(objectUp, glm::vec3(1.0f, 0.0f, 0.0f))) * AngularVelocity.y;
    ///RotationVector += deltaTime * glm::normalize(objectUp) * AngularVelocity.z;

    /**********
    * Rotation Update (Quanternion)
    * AngularVelocity defines the angular velocity for Pitch, Yaw and Roll
    **********/
    glm::quat orientation;
    Pitch += deltaTime * AngularVelocity.x;
    Yaw += deltaTime * AngularVelocity.y;
    Roll += deltaTime * AngularVelocity.z;
    glm::quat qPitch = glm::angleAxis(glm::radians(Pitch), glm::vec3(1,0,0));
    glm::quat qYaw = glm::angleAxis(glm::radians(Yaw), glm::vec3(0,1,0));
    glm::quat qRoll = glm::angleAxis(glm::radians(Roll), glm::vec3(0,0,1));
    orientation = qPitch * qYaw * qRoll;
    glm::mat4 rotateMat = glm::mat4_cast(orientation);

    /**********
    * Translate Update
    * Velocity defines the velocity along the front direction(?)
    **********/
   // glm::vec3 Front = glm::rotate(glm::inverse(orientation), glm::vec3(0.0, 0.0, -1.0));
    //glm::vec3 Right = glm::rotate(orientation, glm::vec3(1.0, 0.0, 0.0));
    //glm::vec3 Up = glm::rotate(glm::inverse(orientation), glm::vec3(1.0, 0.0, 0.0));
    glm::vec3 objectUp;
    objectUp.x = -cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    objectUp.y = sin(glm::radians(Pitch));
    objectUp.z = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    objectUp = glm::normalize(objectUp);
    PositionVector += deltaTime * glm::normalize(objectUp) * Velocity.z;
    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), PositionVector);

    /**********
    * Calculate model matrix based on vector Postion and Rotation
    **********/
    //glm::mat4 m_model = translateMat * rotateMat;
    //m_model = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

    CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = translateMat * rotateMat;
 }

 void CObject::SetRotation(glm::mat4 m){
    //graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//objectList[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    //CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//objectList[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = m;
}

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
    std::cout<<"Set(Sampler) size = "<<descriptorSize<<std::endl;

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
            imageInfo[j].imageView = textureImages[j].textureImageBuffer.view;
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

void CObject::Register(CApplication *p_app, int texture_id, int model_id, int object_id){ //-1 means no texture or model 
    m_object_id = object_id;
    m_texture_id = texture_id;
    m_model_id = model_id;
    bUseMVP_VP = CGraphicsDescriptorManager::CheckMVP();
    
    //Prepare pointers for drawcall
    p_renderer = &(p_app->renderer);
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
    //TODO: bind descriptor set

    //VkDeviceSize offsets[] = { 0 };
    //vkCmdBindVertexBuffers(p_renderer->commandBuffers[p_renderer->graphicsCmdId][p_renderer->currentFrame], 0, 1, &buffer[p_renderer->currentFrame].buffer, offsets);
    p_renderer->BindExternalBuffer(buffer);
    p_renderer->Draw(n);
}
