#ifndef H_TEXTMANAGER
#define H_TEXTMANAGER

#include "object.h"
#include "modelManager.h"

//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;

/******************
* TextBox
*******************/

class CTextBox : public CEntity {
    std::string m_text = "";
public:
    CTextBox();
    void SetText(std::string text){m_text = text;}
};

/******************
* TextManager
*******************/

class CTextManager {
    int m_samplerID = 0;
    glm::vec4 m_color = glm::vec4(255.0f);

public:
    CRenderer *p_renderer;
    CTextImageManager *p_textImageManager;
    CModelManager *p_modelManager;

    const std::string ascII = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    // Map to store glyph texture data for a font
    std::map<char, GlyphTexture> glyphMap;

    CTextManager();
    //~CTextManager();

    void SetFontSize(int fontSize){m_fontSize = fontSize;}
    void SetSamplerID(int samplerID){m_samplerID = samplerID;}
    void SetColor(glm::vec4 color){m_color = color;}

    TTF_Font* m_font = nullptr;
    int m_fontSize = 20;
    void CreateTextFonts();

    //void Initialize();
    //void Update();
    //void Render();

    void CreateTextImage();

    void CreateTextModel();

    void AddTextBox(const CTextBox& textBox);
    //void RemoveTextBox(const CTextBox& textBox);

    void CreateGlyphMap();

private:
    std::vector<CTextBox> m_textBoxes;
};

#endif