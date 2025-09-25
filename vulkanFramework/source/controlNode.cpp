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
void CControlNode::RegisterObject(int startIndex){

}
void CControlNode::RegisterTextbox(int startIndex){

}
void CControlNode::Update(){

}
void CControlNode::Magnet(MagnetTypes type){
   switch(type){
        case MAGNETRIGHT:
            SetPositionX(1.0f - 0.5f * Length.x);
            break;
        case MAGNETLEFT:
            SetPositionX(-1.0f +  0.5f * Length.x);
            break;
        case MAGNETTOP:
            SetPositionY(-1.0f + 0.5f * Length.y);
            break;
        case MAGNETBOTTOM:
            SetPositionY(1.0f - 0.5f * Length.y);
            break;
    }
}


/******************
* ControlPerfMetric
*******************/
CControlPerfMetric::CControlPerfMetric(){    
    m_textbox_count = 8;
    m_object_count = 1;
    //SetPosition(-0.5, -0.5, 0);
    SetRotation(0, 0, 0);
    Name = "Control PerfMetric Node";
    Length_original = glm::vec3(1, 1, 0);
    //SetScaleRectangleXY(0.0, -1.0, 0.5, -0.5); //set position, scale and length
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5 * Length.y);
    Magnet(MAGNETRIGHT);
}

void CControlPerfMetric::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "PerfMetric Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        //objects[objects.size() - 1].m_controlNode_id = 0;
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);

        //no need to set position/scale, because all object follows control node's model matrix

        //does not work
        //object->SetPosition(Position);
        //std::cout<<"ControlPerfMetric Position: "<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;
        //Length_original = glm::vec3(1, 1, 0);
        //object->SetScale(Scale);
        //std::cout<<"ControlPerfMetric Scale: "<<Scale.x<<","<<Scale.y<<","<<Scale.z<<std::endl;
        //object->UpdateLength();

       //std::cout<<"ControlPerfMetric Length: "<<Length.x<<","<<Length.y<<","<<Length.z<<std::endl;
       //std::cout<<"ControlPerfMetric Length_original: "<<Length_original.x<<","<<Length_original.y<<","<<Length_original.z<<std::endl;

        //does not work
        //object->Length_original = glm::vec3(1, 1, 0);
        //object->SetScaleRectangleXY(-0.5, -0.5, 0.0, 0.0);

        //object->SetScaleRectangleXY(Position.x, Position.y, Position.x + Length.x, Position.y + Length.y);
        //object->SetScale(0.48,0.5,0.0);//set scale after model is registered, otherwise the length will not be computed correctly
    }


}

void CControlPerfMetric::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "PerfMetric Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetPosition(-0.15, -0.2+0.05*i, 0); //todo: adjust position based on control node scale
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
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

void CControlPerfMetric::Update(){
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

/******************
* ControlAttachment
*******************/
CControlAttachment::CControlAttachment(){    
    m_textbox_count = 8;
    m_object_count = 1;
    //SetPosition(-0.2, 0.0, 0);
    SetRotation(0, 0, 0);
    Name = "Control Attachment Node";
    //Length = glm::vec3(0.4, 0.5, 0);
    Length_original = glm::vec3(1, 1, 0);
    //SetScaleRectangleXY(0.0,-0.5, 0.5, 0.0); //set position, scale and length
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5 + 0.5 * Length.y);
    Magnet(MAGNETLEFT);
}

void CControlAttachment::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Attachment Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);

        //object->SetScale(0.48,0.5,0.0);//set scale after model is registered, otherwise the length will not be computed correctly
        //object->SetScale(2,2,0.0);
        //object->SetScaleRectangleXY(Position.x, Position.y, Position.x + Length.x, Position.y + Length.y);
    }
}

void CControlAttachment::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Attachment Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetPosition(-0.15, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Attachment";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }else if(i == 1){
            m_pTextboxes[1]->m_text_content = "Renderpass:Shadowmap";
            m_pTextboxes[1]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }else if(i == 3){
            m_pTextboxes[3]->m_text_content = "Renderpass:MainScene";
            m_pTextboxes[3]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlAttachment::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
		int currentTrigger = (double)m_pApp->elapseTime / 0.1f;
		if(currentTrigger!=lastTrigger) {

        std::string r = "false";
        if(m_pApp->renderProcess.iShadowmapAttachmentDepthLight>=0) r = "true"; else r= "false";
        m_pTextboxes[2]->SetTextContent("Depth(Light):" + r);
        if(m_pApp->renderProcess.iMainSceneAttachmentDepthLight>=0) r = "true"; else r= "false";
        m_pTextboxes[4]->SetTextContent("Depth(Light):" + r);
        if(m_pApp->renderProcess.iMainSceneAttachmentDepthCamera>=0) r = "true"; else r= "false";
        m_pTextboxes[5]->SetTextContent("Depth(Camera):" + r);
        if(m_pApp->renderProcess.iMainSceneAttachmentColorResovle>=0) r = "true"; else r= "false";
        m_pTextboxes[6]->SetTextContent("Color(Resolve):" + r);
        if(m_pApp->renderProcess.iMainSceneAttachmentColorPresent>=0) r = "true"; else r= "false";
        m_pTextboxes[7]->SetTextContent("Color(Present):" + r);

        lastTrigger = currentTrigger;
	}
}

/******************
* ControlGraphicsUniform
*******************/
CControlGraphicsUniform::CControlGraphicsUniform(){    
    m_textbox_count = 7;
    m_object_count = 1;
    //SetPosition(-0.4, -0.1, 0);
    SetRotation(0, 0, 0);
    Name = "Control Graphics Uniform Node";
    //Length = glm::vec3(0.4, 0.5, 0);
    Length_original = glm::vec3(1, 1, 0);
    //SetScaleRectangleXY(0.0, 0.0, 0.5, 0.5); //set position, scale and length
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5 + 0.5 + 0.5 * Length.y);
    Magnet(MAGNETBOTTOM);
}

void CControlGraphicsUniform::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Graphics Uniform Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);

        //object->SetScaleRectangleXY(Position.x, Position.y, Position.x + Length.x, Position.y + Length.y);
        //object->SetScale(0.48,0.5,0.0);//set scale after model is registered, otherwise the length will not be computed correctly
    }
}

void CControlGraphicsUniform::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Graphics Uniform Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetPosition(-0.15, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Graphics Uniform";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlGraphicsUniform::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
		int currentTrigger = (double)m_pApp->elapseTime / 0.1f;
		if(currentTrigger!=lastTrigger) {

        std::string r = "false";
        if(m_pApp->appInfo.Uniform.b_uniform_graphics_lighting) r = "true"; else r= "false";
        m_pTextboxes[1]->SetTextContent("Lighting:" + r);
        if(m_pApp->appInfo.Uniform.b_uniform_graphics_mvp) r = "true"; else r= "false";
        m_pTextboxes[2]->SetTextContent("MVP:" + r);
        if(m_pApp->appInfo.Uniform.b_uniform_graphics_text_mvp) r = "true"; else r= "false";
        m_pTextboxes[3]->SetTextContent("Text MVP:" + r);
        if(m_pApp->appInfo.Uniform.b_uniform_graphics_vp) r = "true"; else r= "false";
        m_pTextboxes[4]->SetTextContent("VP:" + r);
        if(m_pApp->appInfo.Uniform.b_uniform_graphics_depth_image_sampler) r = "true"; else r= "false";
        m_pTextboxes[5]->SetTextContent("Depth Image Sampler:" + r);
        if(m_pApp->appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler) r = "true"; else r= "false";
        m_pTextboxes[6]->SetTextContent("Lightdepth Image Sampler:" + r);


        lastTrigger = currentTrigger;
	}
}
