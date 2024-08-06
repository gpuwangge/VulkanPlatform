#ifndef H_OBJECT
#define H_OBJECT
#include "common.h"
#include "context.h"
#include "dataBuffer.hpp"
#include "descriptor.h"

class CObject {
public:
    unsigned int id;

    std::vector<Vertex2D> vertices2D;

    std::vector<Vertex3D> vertices3D;
	std::vector<uint32_t> indices3D;
    
    //CTextureDescriptor textureDescriptor;
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    CObject(){
        id = 0;
    }

    void InitVertices2D(std::vector<Vertex2D> &input_verticices2D){
        vertices2D = input_verticices2D;
    }

    void InitVertices3D(std::vector<Vertex3D> &input_verticices3D){
        vertices3D = input_verticices3D;
    }

    void InitIndices3D(std::vector<uint32_t> &input_indices3D){
        indices3D = input_indices3D;
    }

    // void CreateTextureDescriptor(uint32_t mipLevels, CTextureImage &textureImages){
    //     //textureDescriptor.addImageSamplerUniformBuffer(mipLevels);
	// 	//textureDescriptor.createDescriptorPool();
	// 	//textureDescriptor.createDescriptorSetLayout();
	// 	textureDescriptor.createTextureDescriptorSets(textureImages, descriptorSets);
    // }
    // void CreateTextureDescriptor(uint32_t mipLevels, std::vector<CTextureImage> *textureImages){
    //     //textureDescriptor.addImageSamplerUniformBuffer(mipLevels);
	// 	//textureDescriptor.createDescriptorPool();
	// 	//textureDescriptor.createDescriptorSetLayout();
	// 	textureDescriptor.createTextureDescriptorSets(textureImages, descriptorSets);
    // }

    void createTextureDescriptorSets(CTextureImage &textureImages, VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, VkSampler &sampler, std::vector<VkImageView> *swapchainImageViews = NULL){
        //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;

        int descriptorSize = 1;//getDescriptorSize();
        std::cout<<"Texture descriptor set size (Object) = "<<descriptorSize<<std::endl;

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
            int counter = 0;

            VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform

            std::vector<VkDescriptorImageInfo> imageInfo{}; //for texture sampler
            //if(uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT){
            //imageInfo.resize(textureSamplers.size());
            imageInfo.resize(1);
            //for(int j = 0; j < textureSamplers.size(); j++){
                imageInfo[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL; //test compute storage image: ?need figure this out. VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                imageInfo[0].imageView = textureImages.textureImageBuffer.view;
                imageInfo[0].sampler = sampler;
                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pImageInfo = &imageInfo[0];
                counter++;
            //}
            //}

            //Step 4
            vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

        //std::cout<<"Done set descriptor. "<<std::endl;
    }

    void createTextureDescriptorSets(std::vector<CTextureImage> &textureImages, VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews = NULL){
        //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;

        int descriptorSize = samplers.size();//getDescriptorSize();
        //std::cout<<"createTextureDescriptorSets::samplers.size(): "<<samplers.size()<<std::endl;
        std::cout<<"Texture set size = "<<descriptorSize<<std::endl;

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
            //imageInfo.resize(1);
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

    void createTextureDescriptorSets(std::vector<CTextureImage> *textureImages, VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews = NULL){
        //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;

        int descriptorSize = 1;//getDescriptorSize();
        //std::cout<<"descriptorSize: "<<descriptorSize<<std::endl;

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
            int counter = 0;

            VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform

            std::vector<VkDescriptorImageInfo> imageInfo{}; //for texture sampler
            //if(uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT){
            imageInfo.resize(samplers.size());
            for(int j = 0; j < samplers.size(); j++){
                imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_GENERAL; //test compute storage image: ?need figure this out. VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                imageInfo[j].imageView = (*textureImages)[j].textureImageBuffer.view;
                imageInfo[j].sampler = samplers[j];
                descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[counter].dstSet = descriptorSets[i];
                descriptorWrites[counter].dstBinding = counter;
                descriptorWrites[counter].dstArrayElement = 0;
                descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[counter].descriptorCount = 1;
                descriptorWrites[counter].pImageInfo = &imageInfo[j];
                counter++;
            }
            //}

            //Step 4
            vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

        //std::cout<<"Done set descriptor. "<<std::endl;
    }    

    void CleanUp(){
            //textureDescriptor.DestroyAndFree();
    }
};

#endif