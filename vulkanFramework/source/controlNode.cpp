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
    m_textbox_count = 1;
    m_object_count = 1;
    SetPosition(-0.5, -0.5, 0);
    SetRotation(0, 0, 45);
}

void CPerfMetric::RegisterObject(CApplication *p_app, CObject *object0){
    m_pObject0 = object0;
    object0->bSticker = true;
    object0->SetPosition(0,0,0);
    //objects[objects.size() - 1].m_controlNode_id = 0;
    object0->p_controlNode = this;
    object0->m_object_id = p_app->objects.size() - 1;
    object0->m_texture_ids = std::vector<int>(1,1);
    object0->m_model_id = 0;
    object0->m_default_graphics_pipeline_id = 1;

    object0->Register(p_app);

    object0->SetScale(0.48,0.5,0.5);//set scale after model is registered, otherwise the length will not be computed correctly
}

void CPerfMetric::RegisterTextbox(CApplication *p_app, CTextbox *textbox0){
    m_pTextbox0 = textbox0;
    textbox0->bSticker = true;
    textbox0->SetScale(0.2f);
    textbox0->SetTextColor(glm::vec4(0.2, 0, 0, 1));
    textbox0->SetPosition(-0.5, 0, 0);
    //perfMetric.m_textBoxes[0].Register((CApplication*)this,
    textbox0->p_controlNode = this;
    textbox0->m_textBoxID = p_app->textManager.m_textBoxes.size() - 1;
    textbox0->SetTextContent("text_content");
    textbox0->m_model_id = 1;
    textbox0->m_default_graphics_pipeline_id = 2;

    textbox0->Register(p_app);

    //std::vector<int>(1,0), //resource_text_id_list, 
}

void CPerfMetric::Update(float deltaTime){
    //std::cout<<"CPerfMetric::Update()"<<std::endl;
    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix
}

//void CPerfMetric::Draw(){}





