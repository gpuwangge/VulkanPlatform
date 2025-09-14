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
    object0->SetScale(0.48,0.5,0.5);
    object0->SetPosition(0,0,0);
    //objects[objects.size() - 1].m_controlNode_id = 0;
    object0->p_controlNode = this;
    object0->Register(p_app, 
            p_app->objects.size() - 1, //object_id, 
            std::vector<int>(1,1), //resource_texture_id_list, 
            std::vector<int>(), //isText ? resource_text_id_list : std::vector<int>(),
            0, //resource_model_id, 
            1 //resource_default_graphics_pipeline_id);
        );
}

void CPerfMetric::RegisterTextbox(CApplication *p_app, CTextbox *textbox0){
    m_pTextbox0 = textbox0;
    textbox0->bSticker = true;
    textbox0->SetScale(0.2f);
    textbox0->SetTextColor(glm::vec4(0.2, 0, 0, 1));
    textbox0->SetPosition(-0.5, 0, 0);
    //perfMetric.m_textBoxes[0].Register((CApplication*)this,
    textbox0->p_controlNode = this;
    textbox0->Register(p_app,
        p_app->textManager.m_textBoxes.size() - 1, //textbox_id, 
        std::vector<int>(1,0), //resource_text_id_list, 
        "text_content", 
        1, //resource_model_id, 
        2 //resource_default_graphics_pipeline_id
    );
}

void CPerfMetric::Update(float deltaTime){
    //std::cout<<"CPerfMetric::Update()"<<std::endl;
    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix
}

//void CPerfMetric::Draw(){}





