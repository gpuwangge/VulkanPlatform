#ifndef H_OBJECT
#define H_OBJECT
#include "descriptor.h"

#include <glm/gtc/quaternion.hpp>


//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;

class CObject {
    int m_object_id;
    int m_texture_id;
    int m_model_id;

    bool bUseMVP_VP;

   /******************
    * Tranformation
    *******************/
    glm::vec3 Position; //absolute position coordinate
    glm::vec3 Rotation; //Pitch, Yaw, Roll

    glm::mat4 RotationMatrix; //16*4=64 bytes
    void ComputeRotationMatrix();

    glm::vec3 DirectionFront;
    glm::vec3 DirectionUp;
    glm::vec3 DirectionLeft;
    void ComputeDirections();

    glm::vec3 Velocity;
    glm::vec3 AngularVelocity; //RotationSpeed for x, y and z axis
    //float MovementSpeed;
    //float RotationSpeed;
    //glm::vec3 Accerlation;
    //glm::vec3 Direction;

    enum TranslateDirections {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    glm::vec4 TempVelocity[6];
    enum RotationDirections {
        PITCHUP,
        PITCHDOWN,
        YAWLEFT,
        YAWRIGHT,
        ROLLLEFT,
        ROLLRIGHT
    };
    glm::vec4 TempAngularVelocity[6];

    // float MaxSpeed;
    // float Speed;
    // float MaxRotateSpeed;
    // float RotateSpeed;
    //glm::vec3 TargetPosition;
    //glm::vec3 TargetDirection;

    float Scale;

    // enum ObjectStates {
    //     IDLE,
    //     MOVING
    // };
    // ObjectStates ObjectState;
    
    //float ComputeDifference(glm::vec3 v1, glm::vec3 v2);


    bool bUseTextureSampler;
    std::vector<VkDescriptorSet> descriptorSets; //This descritpor is for texture sampler. one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //These will be used when recording draw cmd
    CRenderer *p_renderer;
    std::vector<VkDescriptorSet> *p_graphicsDescriptorSets;
    VkPipelineLayout *p_graphicsPipelineLayout;

    void CreateTextureDescriptorSets(
        CTextureImage &textureImage, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        VkSampler &sampler, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void CreateTextureDescriptorSets(
        std::vector<CTextureImage> &textureImages, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        std::vector<VkSampler> &samplers, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void CleanUp();

public:
    CObject();

    bool bSticker;

    int GetObjectID(){return m_object_id;}
    int GetTextureID(){return m_texture_id;}
    int GetModelID(){return m_model_id;}

    //void SetRotation(glm::mat4 m);//LEGACY

    void MoveForward(float distance, float speed);
    void MoveBackward(float distance, float speed);
    void MoveLeft(float distance, float speed);
    void MoveRight(float distance, float speed);
    void MoveUp(float distance, float speed);
    void MoveDown(float distance, float speed);

    void PitchUp(float angle, float speed);
    void PitchDown(float angle, float speed);
    void YawLeft(float angle, float speed);
    void YawRight(float angle, float speed);
    void RollLeft(float angle, float speed);
    void RollRight(float angle, float speed);

    void SetPosition(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);

    glm::vec4 TempMoveVelocity;
    void MoveToPosition(float x, float y, float z, float t);
    glm::vec4 TempMoveAngularVelocity;
    void MoveToRotation(float pitch, float yaw, float roll, float t);

    void SetVelocity(float vx, float vy, float vz);
    void SetAngularVelocity(float vx, float vy, float vz);

    bool bUpdate;
    void Update(float deltaTime);

    //when a 3d object is register, it could be with texture(texture_id>=0) or no texture(texture_id=-1); it could have one texture, or several textures(INT_MAX)
    void Register(CApplication *p_app, int texture_id = 0, int model_id = 0, int object_id = 0); //-1 means no texture or model 

    //draw with renderer's buffer, or no buffer
    void Draw(uint32_t n = 0);
    //draw with external buffers
    void Draw(std::vector<CWxjBuffer> &buffer, uint32_t n = 0); //const VkBuffer *pBuffers

};

#endif