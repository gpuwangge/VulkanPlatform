#include "../include/object.h"
#include "../include/application.h"

CObject::CObject(){
    bUseMVP_VP = false;
    bUseTextureSampler = false;
    bUpdate = true;

    //ObjectState = ObjectStates::IDLE;

    Position = glm::vec3();
    Rotation = glm::vec3();

    Velocity = glm::vec3();
    AngularVelocity = glm::vec3();

    for(int i = 0; i < 6; i++) TempVelocity[i] = glm::vec4();
    for(int i = 0; i < 6; i++) TempAngularVelocity[i] = glm::vec4();

    RotationMatrix = glm::mat4();
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

    Scale = 1.0f;
}

 void CObject::Update(float deltaTime){
    if(!bUpdate) return;
    //std::cout<<"Update Object"<<m_object_id<<std::endl;
    //std::cout<<"deltaTime: "<<deltaTime<<std::endl;

    glm::vec3 CurrentVelocity = Velocity;
    glm::vec3 CurrentAngularVelocity = AngularVelocity;
    for(int i = 0; i < 6; i++){
        if(TempVelocity[i].w > 0){
            CurrentVelocity += glm::vec3(TempVelocity[i]);
            TempVelocity[i].w -= deltaTime;
        }
        if(TempAngularVelocity[i].w > 0){
            CurrentAngularVelocity += glm::vec3(TempAngularVelocity[i]);
            TempAngularVelocity[i].w -= deltaTime;
        }
    }
    if(TempMoveVelocity.w > 0){
        CurrentVelocity += glm::vec3(TempMoveVelocity);
        TempMoveVelocity.w -= deltaTime;
    }
    if(TempMoveAngularVelocity.w > 0){
        CurrentAngularVelocity += glm::vec3(TempMoveAngularVelocity);
        TempMoveAngularVelocity.w -= deltaTime;
    }

    /**********
    * Vulkan Screen Coordinate: Right Hand Rule, x from left to right, y from up to bottom, z from far to near 
    **********/
    Rotation += deltaTime * glm::vec3(CurrentAngularVelocity.x, CurrentAngularVelocity.y, CurrentAngularVelocity.z);

    /**********
    * Translate Update
    **********/
    ComputeRotationMatrix();
    ComputeDirections();
    Position += deltaTime * DirectionFront * CurrentVelocity.z;
    Position += deltaTime * DirectionUp * CurrentVelocity.y;
    Position += deltaTime * DirectionLeft * CurrentVelocity.x;
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), Position);

    //std::cout<<"Position = "<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;

    /**********
    * Calculate model matrix based on vector Postion and Rotation
    **********/
    CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = translateMatrix * RotationMatrix;
    
    // switch (ObjectState)
    // {
    //     case ObjectStates::IDLE:
    //         break;
    //     case ObjectStates::MOVING:
    //         if(TempVelocityForward.w > 0){
    //             Velocity = glm::vec3(TempVelocityForward);
    //             TempVelocityForward.w -= deltaTime;
    //         }else {
    //             Velocity = glm::vec3();
    //             ObjectState = ObjectStates::IDLE;
    //         }
    //         // if(ComputeDifference(Position, TargetPosition) < 0.01f){
    //         //     Velocity = glm::vec3();
    //         //     TargetPosition = glm::vec3();
    //         //     ObjectState = ObjectStates::IDLE;
    //         // }
    //         break;
    //     default:
    //         break;
    // }
 }

// float CObject::ComputeDifference(glm::vec3 v1, glm::vec3 v2){
//     return glm::sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
// }

void CObject::ComputeRotationMatrix(){
    /**********
    * Rotation Update (Quanternion)
    * AngularVelocity defines the angular velocity for Pitch, Yaw and Roll
    **********/
    glm::quat orientation;
    glm::quat qPitch = glm::angleAxis(glm::radians(Rotation[0]), glm::vec3(1,0,0));
    glm::quat qYaw = glm::angleAxis(glm::radians(Rotation[1]), glm::vec3(0,1,0));
    glm::quat qRoll = glm::angleAxis(glm::radians(Rotation[2]), glm::vec3(0,0,1));
    orientation = qPitch * qYaw * qRoll; //order matters: first pitch then yaw
    //orientation = qYaw * qPitch * qRoll;  //first yaw then pitch
    RotationMatrix = glm::mat4_cast(orientation);    
}

void CObject::ComputeDirections(){
    DirectionFront = glm::normalize(RotationMatrix * glm::vec4(0,0,1,0));
    DirectionUp = glm::normalize(RotationMatrix * glm::vec4(0,1,0,0));
    DirectionLeft = glm::normalize(RotationMatrix * glm::vec4(1,0,0,0));
}

//void CObject::SetRotation(glm::mat4 m){
    //graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//objectList[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    //CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//objectList[0].SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    
    //CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = m;
//}

void CObject::MoveForward(float distance, float speed){ TempVelocity[TranslateDirections::FORWARD] = glm::vec4(0, 0, speed, distance/speed); }
void CObject::MoveBackward(float distance, float speed){ TempVelocity[TranslateDirections::BACKWARD] = glm::vec4(0, 0, -speed, distance/speed); }
void CObject::MoveLeft(float distance, float speed){ TempVelocity[TranslateDirections::LEFT]= glm::vec4(speed, 0, 0, distance/speed); }
void CObject::MoveRight(float distance, float speed){ TempVelocity[TranslateDirections::RIGHT] = glm::vec4(-speed, 0, 0, distance/speed); }
void CObject::MoveUp(float distance, float speed){ TempVelocity[TranslateDirections::UP]= glm::vec4(0, -speed, 0, distance/speed); }
void CObject::MoveDown(float distance, float speed){ TempVelocity[TranslateDirections::DOWN]= glm::vec4(0, speed, 0, distance/speed); }

void CObject::PitchUp(float angle, float speed){ TempAngularVelocity[RotationDirections::PITCHUP] = glm::vec4(speed, 0, 0, angle/speed); }
void CObject::PitchDown(float angle, float speed){ TempAngularVelocity[RotationDirections::PITCHDOWN] = glm::vec4(-speed, 0, 0, angle/speed); }
void CObject::YawLeft(float angle, float speed){ TempAngularVelocity[RotationDirections::YAWLEFT] = glm::vec4(0, speed, 0, angle/speed); }
void CObject::YawRight(float angle, float speed){ TempAngularVelocity[RotationDirections::YAWRIGHT] = glm::vec4(0, -speed, 0, angle/speed);  }
void CObject::RollLeft(float angle, float speed){ TempAngularVelocity[RotationDirections::ROLLLEFT] = glm::vec4(0, 0, speed, angle/speed); }
void CObject::RollRight(float angle, float speed){ TempAngularVelocity[RotationDirections::ROLLRIGHT] = glm::vec4(0, 0, -speed, angle/speed); }

void CObject::SetPosition(float x, float y, float z){ Position = glm::vec3(x, y, z); }
void CObject::SetRotation(float pitch, float yaw, float roll){ Rotation = glm::vec3(pitch, yaw, roll); }

void CObject::MoveToPosition(float x, float y, float z, float t){
    TempMoveVelocity = glm::vec4((x - Position.x)/t, (y - Position.y)/t, (z - Position.z)/t, t);
}
void CObject::MoveToRotation(float pitch, float yaw, float roll, float t){
    TempMoveAngularVelocity = glm::vec4((pitch - Rotation.x)/t, (yaw - Rotation.y)/t, (roll - Rotation.z)/t, t);
}

void CObject::SetVelocity(float vx, float vy, float vz){ Velocity = glm::vec3(vx, vy, vz); }
void CObject::SetAngularVelocity(float vx, float vy, float vz){ AngularVelocity = glm::vec3(vx, vy, vz); }

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
