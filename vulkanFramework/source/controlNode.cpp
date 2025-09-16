#include "../include/controlNode.h"
#include "../include/application.h"
#include <iostream>

/******************
* ControlNode
*******************/
void CControlNode::Register(CApplication *p_app){
    m_pApp = p_app;
    p_app->objectCountControl += m_object_count;
    p_app->textboxCountControl += m_textbox_count;
    p_app->lightCountControl += m_light_count;
}

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

void CPerfMetric::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->bSticker = true;
        object->SetPosition(0,0,0);
        //objects[objects.size() - 1].m_controlNode_id = 0;
        object->p_controlNode = this;
        object->m_object_id = m_pApp->objects.size() - 1;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);

        object->SetScale(0.48,0.5,0.5);//set scale after model is registered, otherwise the length will not be computed correctly
    }


}

void CPerfMetric::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetPosition(-0.15, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        // textbox->SetTextContent("text_content");
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "PerfMetric";  //->SetTextContent("PerfMetric");
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CPerfMetric::Update(){
    //std::cout<<"CPerfMetric::Update()"<<std::endl;
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
		int currentTrigger = (double)m_pApp->elapseTime / 0.1f;
		if(currentTrigger!=lastTrigger) {

        m_pTextboxes[1]->SetTextContent("FPS:" + std::to_string((int)(1.0f/m_pApp->deltaTime)));
        m_pTextboxes[2]->SetTextContent("Time:" + to_string_prec(m_pApp->elapseTime) + "s");
        m_pTextboxes[3]->SetTextContent("Frame:" + std::to_string(m_pApp->frameCount));
        m_pTextboxes[4]->SetTextContent("Init:" + to_string_prec(m_pApp->totalInitTime) + " ms");
        m_pTextboxes[5]->SetTextContent("Object:" + std::to_string(m_pApp->objects.size()));
        m_pTextboxes[6]->SetTextContent("Textbox:" + std::to_string(m_pApp->textManager.m_textBoxes.size()));
        m_pTextboxes[7]->SetTextContent("Light:" + std::to_string(m_pApp->lights.size()));
        lastTrigger = currentTrigger;
	}
}







