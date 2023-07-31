#ifndef H_VULKANBASE
#define H_VULKANBASE

#include "application.h"

class CVulkanBase : public CApplication{
public:
    CVulkanBase(){}
    ~CVulkanBase(){}

    /**************
    Interface function to vulkan samples
    ************/
    void wxjLoadObjModel(const std::string modelName);

    template <typename T>
    void wxjCreateVertexBuffer(IN std::vector<T> input){
        HERE_I_AM("Init05CreateVertexBuffer");
        VkDeviceSize bufferSize = sizeof(input[0]) * input.size();

        //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexDataBuffer);//allocate vertexDataBuffer bufferSize(decided by vertices3D) memory
        REPORT("InitVertexDataBuffer");
        FillDataBufferHelper(vertexDataBuffer, (void *)(input.data()));//copy vertices3D to vertexDataBuffer
    }

    void wxjCreateIndexBuffer();
    void wxjCreateUniformBuffers();

    void wxjCreateCommandBuffer();

    void wxjCreateSwapChainImagesAndImageViews();

    void wxjCreateVertexShader(std::string shaderName);
    void wxjCreateFragmentShader(std::string shaderName);

    //RenderPass: Attachments
    VkAttachmentDescription colorAttachment{};
    bool bUseColorAttachment = false;
    VkAttachmentDescription depthAttachment{};
    bool bUseDepthAttachment = false;
    VkAttachmentDescription colorAttachmentResolve{};
    bool bUseColorAttachmentResolve = false;
    void wxjCreateColorAttachment(VkImageLayout imageLayout);
    void wxjCreateDepthAttachment();
    void wxjCreateColorAttachmentResolve();
    //RenderPass: Subpass
    VkAttachmentReference colorAttachmentRef{};
    VkAttachmentReference depthAttachmentRef{};
    VkAttachmentReference colorAttachmentResolveRef{};
    VkSubpassDescription subpass{};
    void wxjCreateSubpass();
    //RenderPass:Dependency
    VkSubpassDependency dependency{};
    void wxjCreateDependency(VkPipelineStageFlags srcPipelineStageFlag, VkPipelineStageFlags dstPipelineStageFlag);
    //RenderPass
    void wxjCreateRenderPass();

    void wxjCreateFramebuffers();

    void wxjCreateDescriptorPool(std::vector<VkDescriptorType> &descriptorTypes);
    void wxjCreateDescriptorSetLayout(std::vector<VkDescriptorType> &descriptorTypes, std::vector<VkShaderStageFlagBits>& shaderStageFlagBits);
    void wxjCreateDescriptorSets(std::vector<VkDescriptorType> &descriptorTypes);

    template <typename T>
    void wxjCreateGraphicsPipeline(VkPrimitiveTopology topology){
        HERE_I_AM("wxjCreateGraphicsPipeline");

        VkResult result = VK_SUCCESS;

        //1 Asemble Shader
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        //2 Asemble Vertex Info
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        //NTODO: particle and vertex3d
        // if (pt == PIPELINE_COMPUTE) {
        // 	auto bindingDescription = Particle::getBindingDescription();
        // 	auto attributeDescriptions = Particle::getAttributeDescriptions();

        // 	vertexInputInfo.vertexBindingDescriptionCount = 1;
        // 	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        // 	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        // 	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        // }
        // else {
            //auto bindingDescription = Vertex3D::getBindingDescription();
            //auto attributeDescriptions = Vertex3D::getAttributeDescriptions();

            auto bindingDescription = T::getBindingDescription();
            auto attributeDescriptions = T::getAttributeDescriptions();

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        //}

        //3
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //4
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        //5
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        //6
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = msaaSamples;

        //7
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        // if (pt == PIPELINE_COMPUTE) { //blend particle color in compute pipeline
        // 	colorBlendAttachment.blendEnable = VK_TRUE;
        // 	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        // 	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        // 	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        // 	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        // 	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        // 	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        // }
        // else {
            colorBlendAttachment.blendEnable = VK_FALSE;
        //}

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        //8
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        //9
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // if (pt == PIPELINE_COMPUTE) {
        // 	pipelineLayoutInfo.setLayoutCount = 0;
        // 	pipelineLayoutInfo.pSetLayouts = nullptr;
        // }else {
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        //}

        result = vkCreatePipelineLayout(LOGICAL_DEVICE, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout!");
        REPORT("vkCreatePipelineLayout");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;	//1
        pipelineInfo.pVertexInputState = &vertexInputInfo;	//2
        pipelineInfo.pInputAssemblyState = &inputAssembly;	//3
        pipelineInfo.pViewportState = &viewportState;	//4
        pipelineInfo.pRasterizationState = &rasterizer;	//5
        pipelineInfo.pMultisampleState = &multisampling; //6

        if (bUseDepthAttachment) {
            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.stencilTestEnable = VK_FALSE;
            pipelineInfo.pDepthStencilState = &depthStencil;
        }

        pipelineInfo.pColorBlendState = &colorBlending;	//7 
        pipelineInfo.pDynamicState = &dynamicState;	//8 
        pipelineInfo.layout = pipelineLayout;	//9 
        pipelineInfo.renderPass = renderPass;	//10 
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        result = vkCreateGraphicsPipelines(LOGICAL_DEVICE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
        if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");
        REPORT("vkCreateGraphicsPipelines");

        vkDestroyShaderModule(LOGICAL_DEVICE, fragShaderModule, nullptr);
        vkDestroyShaderModule(LOGICAL_DEVICE, vertShaderModule, nullptr);

        HERE_I_AM("DrawFrame() will begin");
    }

    //void wxjCreateSyncObjects();

    void wxjCreateImage_texture(const std::string texturePath, VkImageUsageFlags usage, OUT MyImageBuffer &textureImageBuffer, OUT int32_t &texWidth, OUT int32_t &texHeight);
    void wxjCreateSampler_texture();
    void wxjCreateImageView(IN VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, int mipLevel, OUT VkImageView &imageView);
    void wxjCreateImage(VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, OUT MyImageBuffer &imageBuffer);

    void wxjBeginCommandBuffer();
    void wxjBeginRenderPass(std::vector<VkClearValue> &clearValues);
    void wxjBindPipeline();
    void wxjSetViewport();
    void wxjSetScissor();
    void wxjBindVertexBuffer();
    void wxjBindIndexBuffer();
    void wxjBindDescriptorSets();
    void wxjDrawIndexed();
    void wxjEndRenderPass();
    void wxjEndCOmmandBuffer();

    void wxjGetMaxUsableSampleCount();
    void wxjCreateMipmaps(IN OUT VkImage image);
    void wxjCreateMipmaps(IN OUT VkImage image, VkImageUsageFlags usage, std::string rainbowCheckerboardTexturePath);
    /**************
    Application operation functions
    ************/
    // void initialize();
    // void update();
    // void recordCommandBuffer();
};

#endif