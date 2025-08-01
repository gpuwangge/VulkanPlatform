#include "../include/object.h"
#include "../include/application.h"

/******************
* Object
*******************/
CObject::CObject(){
    Length_original = glm::vec3();
    LengthMin_original = glm::vec3();
    LengthMax_original = glm::vec3();
    Length = glm::vec3();

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
}

 void CObject::Update(float deltaTime, int currentFrame, Camera &mainCamera, Camera &lightCamera){
    if(!bRegistered)  return;
    if(!bUpdate) return;

    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    /**********
    * Calculate model matrix based on Translation, Rotation and Scale
    **********/
    if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP){
        //update model matrix to ubo
        CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = TranslateMatrix * RotationMatrix * ScaleMatrix;

        //Hack TODO: remove this hack
        // lightCamera.Position = glm::vec3(0.0f, 2.5f, 0.0f);
        // glm::vec3 target = glm::vec3(0.0f, 0.5f, 0.0f);
        // glm::vec3 up     = glm::vec3(0.0f, 0.0f, 1.0f); // 或者 0,1,0，取决于你的 coordinate system
        // lightCamera.matrices.view = glm::lookAt(lightCamera.Position, target, up);
        //lightCamera.matrices.view = glm::mat4(1.0f); //correct

        // std::cout<<"Print debug info:"<<std::endl;
        // std::cout<<"mainCamera.matrices.perspective diagonal:"<<std::endl;
        // std::cout<<mainCamera.matrices.perspective[0][0]<<","<<mainCamera.matrices.perspective[1][1]<<","<<mainCamera.matrices.perspective[2][2]<<std::endl;
        // std::cout<<"lightCamera.matrices.perspective diagonal:"<<std::endl;
        // std::cout<<lightCamera.matrices.perspective[0][0]<<","<<lightCamera.matrices.perspective[1][1]<<","<<lightCamera.matrices.perspective[2][2]<<std::endl;
        // std::cout<<"mainCamera.matrices.view diagonal:"<<std::endl;
        // std::cout<<mainCamera.matrices.view[0][0]<<","<<mainCamera.matrices.view[1][1]<<","<<mainCamera.matrices.view[2][2]<<std::endl;
        // std::cout<<"lightCamera.matrices.view diagonal:"<<std::endl;
        // std::cout<<lightCamera.matrices.view[0][0]<<","<<lightCamera.matrices.view[1][1]<<","<<lightCamera.matrices.view[2][2]<<std::endl;

        //update view and perspective matrices to ubo
        if(!bSticker){
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraProj = mainCamera.matrices.perspective;
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj = lightCamera.matrices.perspective; 
            if(bSkybox) {
                CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraView = glm::mat4(glm::mat3(mainCamera.matrices.view)); //remove translate
                CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView = glm::mat4(glm::mat3(lightCamera.matrices.view));
            }else{
                CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraView = mainCamera.matrices.view;
                CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView = lightCamera.matrices.view;
            }
        }else{
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraProj = glm::mat4(1.0f);
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj = glm::mat4(1.0f);
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraView = glm::mat4(1.0f);
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView = glm::mat4(1.0f);
        }

        // std::cout<<"myUBO.mvpDATA["<<m_object_id<<"] lightCameraProj:"<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][3]<<std::endl;

        // std::cout<<"myUBO.mvpDATA["<<m_object_id<<"] lightCameraView:"<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][3]<<std::endl;

        // std::cout<<"myUBO.mvpDATA["<<m_object_id<<"] model:"<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][3]<<std::endl;


        //memcpy to GPU memory
        memcpy(CGraphicsDescriptorManager::mvpUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::mvpUBO, sizeof(CGraphicsDescriptorManager::mvpUBO));
    }

    if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
        CGraphicsDescriptorManager::vpUBO.view = mainCamera.matrices.view;
        CGraphicsDescriptorManager::vpUBO.proj = mainCamera.matrices.perspective;
        memcpy(CGraphicsDescriptorManager::vpUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::vpUBO, sizeof(CGraphicsDescriptorManager::vpUBO));
    }
 }

// float CObject::ComputeDifference(glm::vec3 v1, glm::vec3 v2){
//     return glm::sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
// }


void CObject::CleanUp(){
    //textureDescriptor.DestroyAndFree();
}

void CObject::CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews){
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
            if(j < m_texture_ids.size()){
                imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
                imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
                imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
                imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            }
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

void CObject::Register(CApplication *p_app, int object_id, std::vector<int> texture_ids, int model_id, int default_graphics_pipeline_id){
    m_object_id = object_id; 
    m_texture_ids = texture_ids; 
    m_model_id = model_id; 
    m_default_graphics_pipeline_id = default_graphics_pipeline_id; 
    //m_graphics_pipeline_id2 = graphics_pipeline_id2; 
    bUseMVP_VP = CGraphicsDescriptorManager::CheckMVP();

    if(p_app->appInfo.VertexBufferType == VertexStructureTypes::TwoDimension || p_app->appInfo.VertexBufferType == VertexStructureTypes::ThreeDimension){
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
    //p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayouts[m_graphics_pipeline_id]);
    p_descriptorSets_graphics_general = &(p_app->graphicsDescriptorManager.descriptorSets_general);//?
    p_textureManager = &(p_app->textureManager);


    //there are up to 3 samplers, support up to 3 different textures
    //for(int i = 0; i < m_texture_ids.size(); i++)
    //    std::cout<<"Register object:"<<m_object_id<<" with texture:"<<m_texture_ids[i]<<" and model:"<<model_id<<" and graphcis pipeline:"<<m_graphics_pipeline_id<<std::endl;

    //bUseTextureSampler = true; 
    if(m_texture_ids.size() > 0){
        CreateDescriptorSets_TextureImageSampler(
            CGraphicsDescriptorManager::graphicsDescriptorPool,
            CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler,
            CGraphicsDescriptorManager::textureImageSamplers
        );
    }

    bRegistered = true;
}


void CObject::Draw(int graphicsPipelineId, uint32_t n, bool bSetDepthbias){
    if(!bRegistered || !bVisible) return;

    //std::cout<<"CObject::Draw, renderPassType="<<renderPassType<<", n="<<n<<std::endl;

    // int graphicsPipelineId = m_graphics_pipeline_id_mainscene;
    // switch(renderPassType){
    //     case RenderPassTypes::MAINSCENE:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene;
    //         break;
    //     case RenderPassTypes::MAINSCENE2:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene2;
    //         break;
    //     case RenderPassTypes::SHADOWMAP:
    //         graphicsPipelineId = m_graphics_pipeline_id_shadowmap;
    //         break;
    //     default:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene; //default to mainscene
    // }

    int current_graphics_pipeline_id = (graphicsPipelineId == -1) ? m_default_graphics_pipeline_id : graphicsPipelineId;

    VkPipelineLayout *p_graphicsPipelineLayout = &(p_renderProcess->graphicsPipelineLayouts[current_graphics_pipeline_id]);
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[current_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //if(bSetDepthbias){
        //std::cout<<"Set DepthBias for object:"<<m_object_id<<", graphicsPipelineId:"<<current_graphics_pipeline_id<<std::endl;
        //vkCmdSetDepthBias( p_renderer->commandBuffers[ p_renderer->graphicsCmdId][ p_renderer->currentFrame], 1.25f, 0.0f, 1.75f);
       // vkCmdSetDepthBias( p_renderer->commandBuffers[ p_renderer->graphicsCmdId][ p_renderer->currentFrame], 1.25f-0.49f, 0.0f, 1.75f);
        //vkCmdSetDepthBias( p_renderer->commandBuffers[ p_renderer->graphicsCmdId][ p_renderer->currentFrame], 180.0f, 0.0f, 40.0f);
    //}

    //std::cout<<"test2. p_graphicsDescriptorSets->size()="<<p_graphicsDescriptorSets->size()<<std::endl;
    //std::cout<<"test2. m_texture_ids.size()="<<m_texture_ids.size()<<std::endl;

    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    //set = 0 is for general uniform; set = 1 is for texture sampler uniform
    if(CGraphicsDescriptorManager::getSetSize_General() > 0) dsSets.push_back(*p_descriptorSets_graphics_general); 
    if(CGraphicsDescriptorManager::textureImageSamplers.size() > 0) dsSets.push_back(descriptorSets_graphics_texture_image_sampler); 
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


void CObject::Draw(std::vector<CWxjBuffer> &buffer, int graphicsPipelineId, uint32_t n){ //const VkBuffer *pBuffers
    if(!bRegistered || !bVisible) return;

    // int graphicsPipelineId = m_graphics_pipeline_id_mainscene;
    //     switch(renderPassType){
    //     case RenderPassTypes::MAINSCENE:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene;
    //         break;
    //     case RenderPassTypes::MAINSCENE2:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene2;
    //         break;
    //     case RenderPassTypes::SHADOWMAP:
    //         graphicsPipelineId = m_graphics_pipeline_id_shadowmap;
    //         break;
    //     default:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene; //default to mainscene
    // }
    
    int current_graphics_pipeline_id = (graphicsPipelineId == -1) ? m_default_graphics_pipeline_id : graphicsPipelineId;
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[current_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //this function is used in sample:simpleparticle only
    //std::cout<<"testdraw1,"<<m_graphics_pipeline_id<<","<<p_renderer->graphicsCmdId<<std::endl;

    // if(graphicsPipelineId != -1)
    //     p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[graphicsPipelineId], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);
    // else
    //     p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[m_default_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //TODO: bind descriptor set
    //std::cout<<"testdraw2"<<std::endl;

    //VkDeviceSize offsets[] = { 0 };
    //vkCmdBindVertexBuffers(p_renderer->commandBuffers[p_renderer->graphicsCmdId][p_renderer->currentFrame], 0, 1, &buffer[p_renderer->currentFrame].buffer, offsets);
    p_renderer->BindExternalBuffer(buffer);
    p_renderer->Draw(n);
    //std::cout<<"testdraw3"<<std::endl;
}
