#ifndef H_ENTITY
#define H_ENTITY
#include <glm/gtc/quaternion.hpp>

class CEntity{
public:
    CEntity(){
        DirectionLeft = glm::vec3(1, 0, 0);
        DirectionUp = glm::vec3(0, 1, 0);
        DirectionFront = glm::vec3(0, 0, 1);
    }

    ~CEntity(){

    }

    glm::vec3 Length_original; //original is the value before scale
    glm::vec3 LengthMin_original;
    glm::vec3 LengthMax_original;
    glm::vec3 Length; //length after scale, the true "length"

    /******************
    * Tranformation
    *******************/
    glm::vec3 Position; //absolute position coordinate
    glm::vec3 Rotation; //Pitch, Yaw, Roll
    glm::vec3 Scale;

    glm::mat4 RotationMatrix; //16*4=64 bytes

    glm::mat4 ScaleMatrix;

    glm::mat4 TranslateMatrix;

    glm::vec3 DirectionFront;
    glm::vec3 DirectionUp;
    glm::vec3 DirectionLeft;

    glm::vec3 Velocity;
    glm::vec3 AngularVelocity; //RotationSpeed for x, y and z axis

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

    void SetScale(float scale_x, float scale_y, float scale_z);
    void SetScaleRectangleXY(float x0, float y0, float x1, float y1); //set 2d image to rect((x0,y0),(x1,y1))
    void UpdateLength();

    void Update(float deltaTime);
};

#endif