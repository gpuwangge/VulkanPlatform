#ifndef H_TEXTMANAGER
#define H_TEXTMANAGER

#include "object.h"
#include "modelManager.h"

//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;


/*************
* Character
**************/

// class CCharacter{
//     int m_instanceCount = 0;
// public:
//     CRenderer *p_renderer;
//     CRenderProcess *p_renderProcess;
//     std::vector<VkDescriptorSet> *p_descriptorSets_graphics_general;
//     std::vector<VkDescriptorSet> descriptorSets_graphics_texture_image_sampler;
//     CTextImageManager *p_textImageManager;

//     void SetInstanceCount(int count) { m_instanceCount = count; }

//     CCharacter(){}
//     ~CCharacter(){}

//     void CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews = NULL);

//     void Draw();
// };

/******************
* TextBox
*******************/
class CTextManager;
class CTextBox : public CEntity {
    int m_textBoxID = 0;
    
    int m_instanceCount = 0;

    CRenderer *p_renderer;
    CRenderProcess *p_renderProcess;
    std::vector<VkDescriptorSet> *p_descriptorSets_graphics_general;
    std::vector<VkDescriptorSet> descriptorSets_graphics_texture_image_sampler;
    CTextImageManager *p_textImageManager;

    int m_default_graphics_pipeline_id = 0;
    int m_model_id = 0;
public:
    std::string m_text_content = "";
    std::vector<TextInstanceData> textInstanceData;

    bool bRegistered = false;
    glm::vec4 m_boxColor = glm::vec4(255.0f);
    glm::vec4 m_textColor = glm::vec4(0.0f);
    //std::vector<CCharacter> m_characters;
    void CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews = NULL);

    CTextBox(){}
    void SetText(std::string text_content){m_text_content = text_content;}
    void SetBoxColor(glm::vec4 color){m_boxColor = color;}
    void SetTextColor(glm::vec4 color){m_textColor = color;}
    void CreateTextInstanceData(CTextManager *p_textManager);
    void Register(CApplication *p_app, int textbox_id, std::vector<int> text_ids, std::string text_content, int model_id, int default_graphics_pipeline_id);
    void Update(float deltaTime, int currentFrame, Camera &mainCamera);
    void Draw();
    //     for(auto& ch : m_characters){ 
    //         ch.Draw(); 
    //     }
    // }
};

/******************
* TextManager
*******************/

class CTextManager {
    int m_samplerID = 0;
    //int m_instanceCount = 0;
    glm::vec4 m_color = glm::vec4(255.0f);
    
public:
    std::vector<CTextBox> m_textBoxes;

    CRenderer *p_renderer = NULL;
    CTextImageManager *p_textImageManager = NULL;
    CModelManager *p_modelManager = NULL;

    const std::string ascII = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    // Map to store glyph texture data for a font
    std::map<char, GlyphTexture> glyphMap;

    CTextManager();
    //~CTextManager();

    void SetFontSize(int fontSize){m_fontSize = fontSize;}
    void SetSamplerID(int samplerID){m_samplerID = samplerID;}
    void SetColor(glm::vec4 color){m_color = color;}

    //int GetInstanceCount() const { return m_instanceCount; }
    std::vector<TextQuadVertex> textQuadVertices;

    TTF_Font* m_font = nullptr;
    int m_fontSize = 20;
    void CreateTextFonts();

    void CreateTextImage();
    void CreateTextResource();

    //void AddTextBox(const CTextBox& textBox){ m_textBoxes.push_back(textBox); }
    //void RemoveTextBox(const CTextBox& textBox);

    void CreateGlyphMap();

    //void Initialize();
    void Update(float deltaTime, int currentFrame, Camera &mainCamera);
    void Draw(){for(auto& textBox : m_textBoxes){textBox.Draw();}};

};

#endif