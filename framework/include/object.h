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

    CObject();
    void CleanUp();

    void InitVertices2D(std::vector<Vertex2D> &input_verticices2D);
    void InitVertices3D(std::vector<Vertex3D> &input_verticices3D);
    void InitIndices3D(std::vector<uint32_t> &input_indices3D);

    void createTextureDescriptorSets(
        CTextureImage &textureImage, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        VkSampler &sampler, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void createTextureDescriptorSets(
        std::vector<CTextureImage> &textureImages, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        std::vector<VkSampler> &samplers, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

};

#endif