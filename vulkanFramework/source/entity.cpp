#include "../include/entity.h"
//#include "../include/application.h"
#include <iostream>

/******************
* Entity
*******************/
void CEntity::MoveForward(float distance, float speed){ TempVelocity[TranslateDirections::FORWARD] = glm::vec4(0, 0, speed, distance/speed); }
void CEntity::MoveBackward(float distance, float speed){ TempVelocity[TranslateDirections::BACKWARD] = glm::vec4(0, 0, -speed, distance/speed); }
void CEntity::MoveLeft(float distance, float speed){ TempVelocity[TranslateDirections::LEFT]= glm::vec4(speed, 0, 0, distance/speed); }
void CEntity::MoveRight(float distance, float speed){ TempVelocity[TranslateDirections::RIGHT] = glm::vec4(-speed, 0, 0, distance/speed); }
void CEntity::MoveUp(float distance, float speed){ TempVelocity[TranslateDirections::UP]= glm::vec4(0, -speed, 0, distance/speed); }
void CEntity::MoveDown(float distance, float speed){ TempVelocity[TranslateDirections::DOWN]= glm::vec4(0, speed, 0, distance/speed); }

void CEntity::PitchUp(float angle, float speed){ TempAngularVelocity[RotationDirections::PITCHUP] = glm::vec4(speed, 0, 0, angle/speed); }
void CEntity::PitchDown(float angle, float speed){ TempAngularVelocity[RotationDirections::PITCHDOWN] = glm::vec4(-speed, 0, 0, angle/speed); }
void CEntity::YawLeft(float angle, float speed){ TempAngularVelocity[RotationDirections::YAWLEFT] = glm::vec4(0, speed, 0, angle/speed); }
void CEntity::YawRight(float angle, float speed){ TempAngularVelocity[RotationDirections::YAWRIGHT] = glm::vec4(0, -speed, 0, angle/speed);  }
void CEntity::RollLeft(float angle, float speed){ TempAngularVelocity[RotationDirections::ROLLLEFT] = glm::vec4(0, 0, speed, angle/speed); }
void CEntity::RollRight(float angle, float speed){ TempAngularVelocity[RotationDirections::ROLLRIGHT] = glm::vec4(0, 0, -speed, angle/speed); }

void CEntity::SetPosition(float x, float y, float z){ Position = glm::vec3(x, y, z); }
void CEntity::SetRotation(float pitch, float yaw, float roll){ Rotation = glm::vec3(pitch, yaw, roll); }

void CEntity::MoveToPosition(float x, float y, float z, float t){
    TempMoveVelocity = glm::vec4((x - Position.x)/t, (y - Position.y)/t, (z - Position.z)/t, t);
}
void CEntity::MoveToRotation(float pitch, float yaw, float roll, float t){
    TempMoveAngularVelocity = glm::vec4((pitch - Rotation.x)/t, (yaw - Rotation.y)/t, (roll - Rotation.z)/t, t);
}

void CEntity::SetVelocity(float vx, float vy, float vz){ Velocity = glm::vec3(vx, vy, vz); }
void CEntity::SetAngularVelocity(float vx, float vy, float vz){ AngularVelocity = glm::vec3(vx, vy, vz); }

void CEntity::SetScale(float scale_x, float scale_y, float scale_z){
    Scale = glm::vec3(scale_x, scale_y, scale_z);
    UpdateLength();
}
void CEntity::SetScaleRectangleXY(float x0, float y0, float x1, float y1){
    //Screen Coordinate is (-1,-1) to (1,1)
    std::cout<<"Position: "<<Position.x<<", "<<Position.y<<", "<<Position.z<<std::endl;
    std::cout<<"Length_original: "<<Length_original.x<<", "<<Length_original.y<<", "<<Length_original.z<<std::endl;
    std::cout<<"Scale: "<<Scale.x<<", "<<Scale.y<<", "<<Scale.z<<std::endl;

    //Step 1: Update Scale
    glm::vec3 destinationLength = glm::vec3(x1-x0, y1-y0, 0);
    Scale = glm::vec3(destinationLength.x/Length_original.x, destinationLength.y/Length_original.y, 1);

    //Step 2: Update Position
    Position = glm::vec3(x0+destinationLength.x/2, y0+destinationLength.y/2, 0);

    std::cout<<"Position: "<<Position.x<<", "<<Position.y<<", "<<Position.z<<std::endl;
    std::cout<<"Length_original: "<<Length_original.x<<", "<<Length_original.y<<", "<<Length_original.z<<std::endl;
    std::cout<<"Scale: "<<Scale.x<<", "<<Scale.y<<", "<<Scale.z<<std::endl;

    UpdateLength();
}
void CEntity::UpdateLength(){
    Length = glm::vec3(Length_original.x * Scale.x, Length_original.y * Scale.y, Length_original.z * Scale.z);
    std::cout<<"Length Updated: "<<Length.x<<", "<<Length.y<<", "<<Length.z<<std::endl;
}

void CEntity::Update(float deltaTime){
    glm::vec3 CurrentVelocity = Velocity;
    glm::vec3 CurrentAngularVelocity = AngularVelocity;
    for(int i = 0; i < 6; i++){
        if(TempVelocity[i].w > 0){
            CurrentVelocity += glm::vec3(TempVelocity[i]);
            TempVelocity[i].w -= deltaTime;
        }
        if(TempAngularVelocity[i].w > 0){
            CurrentAngularVelocity += glm::vec3(TempAngularVelocity[i]);
            TempAngularVelocity[i].w -= deltaTime;
        }
    }
    if(TempMoveVelocity.w > 0){
        CurrentVelocity += glm::vec3(TempMoveVelocity);
        TempMoveVelocity.w -= deltaTime;
    }
    if(TempMoveAngularVelocity.w > 0){
        CurrentAngularVelocity += glm::vec3(TempMoveAngularVelocity);
        TempMoveAngularVelocity.w -= deltaTime;
    }

    /**********
    * Vulkan Screen Coordinate: Right Hand Rule, x from left to right, y from up to bottom, z from far to near 
    **********/
    Rotation += deltaTime * glm::vec3(CurrentAngularVelocity.x, CurrentAngularVelocity.y, CurrentAngularVelocity.z);

    /**********
    * Translate Update (Step 1.2.3.)
    **********/
    /**********
    * 1. Compute RotationMatrix (from Quanternion)
    * AngularVelocity defines the angular velocity for Pitch, Yaw and Roll
    * Each component of Rotation is for Pitch, Yaw and Roll
    **********/
    glm::quat orientation;
    glm::quat qPitch = glm::angleAxis(glm::radians(Rotation[0]), glm::vec3(1,0,0));
    glm::quat qYaw = glm::angleAxis(glm::radians(Rotation[1]), glm::vec3(0,1,0));
    glm::quat qRoll = glm::angleAxis(glm::radians(Rotation[2]), glm::vec3(0,0,1));
    //glm::quat qPitch = glm::angleAxis(glm::radians(Rotation[0]), glm::vec3(1,0,0));
    //glm::quat qYaw = glm::angleAxis(glm::radians(Rotation[1]),  DirectionUp);
    //glm::quat qRoll = glm::angleAxis(glm::radians(Rotation[2]), glm::vec3(0,0,1));
    orientation = qPitch * qYaw * qRoll; //order matters: first pitch then yaw
    //orientation = qYaw * qPitch * qRoll;  //first yaw then pitch
    RotationMatrix = glm::mat4_cast(orientation);  

    /**********
    * 2. Compute Directions
    **********/
    DirectionLeft = glm::normalize(RotationMatrix * glm::vec4(1,0,0,0));
    DirectionUp = glm::normalize(RotationMatrix * glm::vec4(0,1,0,0));
    DirectionFront = glm::normalize(RotationMatrix * glm::vec4(0,0,1,0));
    
    

    //std::cout<<"DirectionFront="<<DirectionFront.x<<","<<DirectionFront.y<<","<<DirectionFront.z<<std::endl;

    /**********
    * 3. Compute TranslationMatrix
    **********/
    Position += deltaTime * DirectionFront * CurrentVelocity.z;
    Position += deltaTime * DirectionUp * CurrentVelocity.y;
    Position += deltaTime * DirectionLeft * CurrentVelocity.x;
    TranslateMatrix = glm::translate(glm::mat4(1.0f), Position);

    //std::cout<<"Position = "<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;

    /**********
    * Scale Update
    **********/
    ScaleMatrix[0][0] = Scale.x;
    ScaleMatrix[1][1] = Scale.y;
    ScaleMatrix[2][2] = Scale.z;
    ScaleMatrix[3][3] = 1;
}

