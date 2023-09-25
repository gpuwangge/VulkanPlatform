#ifndef H_RENDERPROCESS
#define H_RENDERPROCESS

#include "common.h"
#include "context.h"
#include "dataBuffer.hpp"


class CRenderProcess final{
public:
    CRenderProcess();
    ~CRenderProcess();

    void Cleanup();

    bool bUseColorAttachment;
    bool bUseDepthAttachment;
    bool bUseColorAttachmentResolve;

    VkAttachmentDescription colorAttachment{};
    VkAttachmentDescription depthAttachment{};
    VkAttachmentDescription colorAttachmentResolve{};

    VkAttachmentReference colorAttachmentRef{};
    VkAttachmentReference depthAttachmentRef{};
    VkAttachmentReference colorAttachmentResolveRef{};

    VkSubpassDescription subpass{};
    VkSubpassDependency dependency{};
    VkRenderPass renderPass; 

    void createSubpass();
    void createDependency(
        VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
        VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    void createRenderPass();

    void addColorAttachment(VkFormat swapChainImageFormat,
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT, 
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    void addDepthAttachment();
    void addColorAttachmentResolve();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();

    VkSampleCountFlagBits m_msaaSamples;
    VkFormat m_swapChainImageFormat;

    bool bCreateGraphicsPipeline = false;
    VkPipelineLayout graphicsPipelineLayout;
	VkPipeline graphicsPipeline;

    bool bCreateComputePipeline = false;
    VkPipelineLayout computePipelineLayout;
	VkPipeline computePipeline;

    void createLayout(VkDescriptorSetLayout &descriptorSetLayout);
    void createLayout(VkDescriptorSetLayout &descriptorSetLayout, VkPushConstantRange &pushConstantRange, bool bUsePushConstant = true);

    struct DummyVertex {
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            return bindingDescription;
        }
        static std::array<VkVertexInputAttributeDescription, 0> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 0> attributeDescriptions{};
            return attributeDescriptions;
        }
    };

    /*******
     * 
     * Graphics Pipeline Template
     * 
     * 
     * *****/
    //this function is for samples that are NOT using vertex shader
    void createGraphicsPipeline(VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule){
        createGraphicsPipeline<DummyVertex>(topology, vertShaderModule, fragShaderModule, false); //DummyVertex doesn't really matter here, because no vertex attributes used
    }

    //this function is for samples that are  using vertex shader
    template <typename T>
    void createGraphicsPipeline(VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, bool bUseVertexBuffer = true){
        //HERE_I_AM("CreateGraphicsPipeline");
        bCreateGraphicsPipeline = true;

        VkResult result = VK_SUCCESS;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
       

        /*********1 Asemble Shader**********/
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
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        
        /*********2 Asemble Vertex Info**********/
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        auto bindingDescription = T::getBindingDescription();
        auto attributeDescriptions = T::getAttributeDescriptions();
        if(bUseVertexBuffer){     
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
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
            //}
        }   
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineInfo.pVertexInputState = &vertexInputInfo;

        /*********3 Assemble**********/
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        pipelineInfo.pInputAssemblyState = &inputAssembly;

        /*********4 Viewport**********/
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        pipelineInfo.pViewportState = &viewportState;
        

        /*********5 Rasterizazer**********/
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        pipelineInfo.pRasterizationState = &rasterizer;	
        

        /*********6 Multisample**********/
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = m_msaaSamples;
        pipelineInfo.pMultisampleState = &multisampling; 

        /*********7 Color Blend**********/
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
        pipelineInfo.pColorBlendState = &colorBlending;	//7 
        

        /*********8**********/
        //tell gpu which part will be changed, so need update these for each frame
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
        pipelineInfo.pDynamicState = &dynamicState;	//8 
         

        /*********9 Layout(Vulkan Special Concept)**********/
        //CreateLayout(descriptorSetLayout);
        pipelineInfo.layout = graphicsPipelineLayout;	//9
        
        /*********10 Renderpass Layout(Vulkan Special Concept)**********/
        //Renderpass is to specify what kind of data goes to graphics pipeline
        pipelineInfo.renderPass = renderPass;	//10 
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        /*********11**********/
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


        /*********Create Graphics Pipeline**********/
        result = vkCreateGraphicsPipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
        if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");
        //REPORT("vkCreateGraphicsPipelines");


        /*********Clean up**********/
        

        //HERE_I_AM("DrawFrame() will begin");
    }


    void createComputePipeline(VkShaderModule &computeShaderModule, VkDescriptorSetLayout &computeDescriptorSetLayout);


    //CDebugger * debugger;
};


#endif