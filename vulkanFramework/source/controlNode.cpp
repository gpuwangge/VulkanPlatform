#include "../include/controlNode.h"
#include "../include/application.h"
#include <iostream>

/******************
* ControlNode
*******************/
//void CControlNode::Register(CApplication *p_app){}

void CControlNode::Update(float deltaTime){

}

//void CControlNode::Draw(){}


/******************
* PerfMetric
*******************/
CPerfMetric::CPerfMetric(){    
    m_textbox_count = 8;
    m_object_count = 1;
    SetPosition(0.3, -0.5, 0);
    SetRotation(0, 0, 30);
}

void CPerfMetric::RegisterObject(CApplication *p_app, CObject *object0){
    m_pObject0 = object0;
    object0->bSticker = true;
    object0->SetPosition(0,0,0);
    //objects[objects.size() - 1].m_controlNode_id = 0;
    object0->p_controlNode = this;
    object0->m_object_id = p_app->objects.size() - 1;
    object0->m_texture_ids = p_app->appInfo.ControlUIContainer.resource_texture_id_list_box;
    object0->m_model_id = p_app->appInfo.ControlUIContainer.resource_model_id_box;
    object0->m_default_graphics_pipeline_id = p_app->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

    object0->Register(p_app);

    object0->SetScale(0.48,0.5,0.5);//set scale after model is registered, otherwise the length will not be computed correctly
}

void CPerfMetric::RegisterTextbox(CApplication *p_app, std::vector<CTextbox> *pTextBoxes, int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(*pTextBoxes)[index];
        m_pTextboxes.push_back(textbox);

        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetPosition(-0.15, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        // textbox->SetTextContent("text_content");
        textbox->m_text_content = "text_content";
        textbox->m_model_id = p_app->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = p_app->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "PerfMetric";  //->SetTextContent("PerfMetric");
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }
        
        textbox->Register(p_app);
    }
    
    // m_pTextbox0 = textbox0;
    // textbox0->bSticker = true;
    // textbox0->SetScale(0.2f);
    // textbox0->SetTextColor(glm::vec4(0.2, 0, 0, 1));
    // textbox0->SetPosition(-0.4, 0, 0);
    // textbox0->p_controlNode = this;
    // textbox0->m_textBoxID = p_app->textManager.m_textBoxes.size() - 2;
    // textbox0->SetTextContent("text_content");
    // textbox0->m_model_id = p_app->appInfo.ControlUIContainer.resource_model_id_text;
    // textbox0->m_default_graphics_pipeline_id = p_app->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
    // textbox0->Register(p_app);

    // m_pTextbox1 = textbox1;
    // textbox1->bSticker = true;
    // textbox1->SetScale(0.2f);
    // textbox1->SetTextColor(glm::vec4(0.2, 0, 0, 1));
    // textbox1->SetPosition(-0.5, 0.05, 0);
    // textbox1->p_controlNode = this;
    // textbox1->m_textBoxID = p_app->textManager.m_textBoxes.size() - 1;
    // textbox1->SetTextContent("text_content");
    // textbox1->m_model_id = p_app->appInfo.ControlUIContainer.resource_model_id_text;
    // textbox1->m_default_graphics_pipeline_id = p_app->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
    // textbox1->Register(p_app);
}

void CPerfMetric::Update(float deltaTime){
    //std::cout<<"CPerfMetric::Update()"<<std::endl;
    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix
}

//void CPerfMetric::Draw(){}





