#include "../include/light.h"
//#include "../include/application.h"

void CLight::Update(float deltaTime, int currentFrame, Camera &mainCamera){
    if(!bRegistered) return;
    if(!bUpdate) return;

    //update camera pos to ubo, then memcpy to GPU memory
    if(CGraphicsDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_LIGHTING_GRAPHICS_BIT){
        //update light pos and intensity to ubo
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].lightPos = glm::vec4(m_position, 0);
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].ambientIntensity = m_intensity[0];
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].diffuseIntensity = m_intensity[1];
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].specularIntensity = m_intensity[2];
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].dimmerSwitch = m_intensity[3];
        //update camera to ubo
        CGraphicsDescriptorManager::m_lightingUBO.cameraPos = glm::vec4(mainCamera.Position, 0);
        //memcpy to GPU memory 
        memcpy(CGraphicsDescriptorManager::m_lightingUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::m_lightingUBO, sizeof(CGraphicsDescriptorManager::m_lightingUBO));
    }

}

void CLight::Register(std::string lightName, int lightId, glm::vec3 lightPos, std::vector<float> &lightIntensity){
    m_name = lightName;
    m_light_id = lightId;
    SetLightPosition(lightPos);
    SetLightIntensity(lightIntensity);
    bRegistered = true;
}