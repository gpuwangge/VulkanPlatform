#ifndef H_CAMERA
#define H_CAMERA

#include "entity.h"

class Camera : public CEntity{
public:
	enum CameraType { LOCK, FREE, SELECT }; //SELECT is not implemented yet
	CameraType cameraType = CameraType::LOCK;
	int focusObjectId = 0; //this is the object that camera is focusing on, it will be used in LOCK mode

	float fov;
	float znear, zfar;
	// float keyboard_sensitive = 60;
  	// float mouse_sensitive = 3;

	//bool chickenbit = false;
	bool bEnableOrthographic = false; //if true, use orthographic projection, otherwise use perspective projection

	glm::vec3 TargetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	void SetTargetPosition(float x, float y, float z){ TargetPosition = glm::vec3(x, y, z); }
	void SetTargetPosition(glm::vec3 pos){ TargetPosition = pos; }

	Camera(){
		entityType = EntityType::camera;
		setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
	}

	struct{
		glm::mat4 projection;
		glm::mat4 view;
	} matrices;

	struct{
		bool forward = false;
		bool backward = false;
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;
		
		bool pitchup = false;
		bool pitchdown = false;
		bool yawleft = false;
		bool yawright = false;
		bool rollleft = false;
		bool rollright = false;
	} state;

	//this function is provided to user
	void setPerspective(float fov, float aspect, float znear, float zfar){ 
		this->fov = fov;
		this->znear = znear;
		this->zfar = zfar;
		matrices.projection = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		//if (flipY) matrices.projection[1][1] *= -1.0f;
	};

	void setOrthographic(float left, float right, float bottom, float top, float znear, float zfar){
		matrices.projection = glm::ortho(left, right, bottom, top, znear, zfar);
		//matrices.perspective = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, -6.0f, -1.5f);//hack
		
		// std::cout<<"matrices.perspective="<<std::endl;
		// std::cout<<matrices.perspective[0][0]<<","<<matrices.perspective[0][1]<<","<<matrices.perspective[0][2]<<","<<matrices.perspective[0][3]<<std::endl;
		// std::cout<<matrices.perspective[1][0]<<","<<matrices.perspective[1][1]<<","<<matrices.perspective[1][2]<<","<<matrices.perspective[1][3]<<std::endl;
		// std::cout<<matrices.perspective[2][0]<<","<<matrices.perspective[2][1]<<","<<matrices.perspective[2][2]<<","<<matrices.perspective[2][3]<<std::endl;
		// std::cout<<matrices.perspective[3][0]<<","<<matrices.perspective[3][1]<<","<<matrices.perspective[3][2]<<","<<matrices.perspective[3][3]<<std::endl;
		// std::cout << glm::to_string(matrices.perspective) << std::endl;

		//std::cout<<"Camera setOrthographic: left="<<left<<", right="<<right<<", bottom="<<bottom<<", top="<<top<<", znear="<<znear<<", zfar="<<zfar<<std::endl;

	}

	//this function is provided to user (no use case yet)
	void updateAspectRatio(float aspect){
		matrices.projection = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		//if (flipY) matrices.projection[1][1] *= -1.0f;
	}

	void update(float deltaTime){
		
		if(cameraType == CameraType::LOCK){ //calculate angular velocity so focus is on target
			glm::vec3 cameraPos2TargetPos = TargetPosition - Position;
			AngularVelocity.x = 2000 * glm::dot(glm::cross(DirectionFront, cameraPos2TargetPos), DirectionLeft);
			AngularVelocity.y = 2000 * glm::dot(glm::cross(cameraPos2TargetPos, DirectionFront), DirectionUp);
		}

		CEntity::Update(deltaTime); //update TranslateMatrix RotationMatrix ScaleMatrix

		//hack
		/*
		if(bEnableOrthographic){
			// DirectionFront = glm::vec3(0,1,0);

			// std::cout<<"Position="<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;
			// std::cout<<"DirectionFront="<<DirectionFront.x<<","<<DirectionFront.y<<","<<DirectionFront.z<<std::endl;
			// std::cout<<"DirectionUp="<<DirectionUp.x<<","<<DirectionUp.y<<","<<DirectionUp.z<<std::endl;

			matrices.view = glm::lookAt(Position, Position + DirectionFront, DirectionUp);

			//glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);
			//glm::vec3 lightTarget = glm::vec3(0.0f, 2.0f, 0.0f);
			//matrices.view = glm::lookAt(lightPosition, lightTarget, glm::vec3(0, 0, 1));
		}else{
			matrices.view = glm::lookAt(Position, Position + DirectionFront, DirectionUp);
		}
		*/

		//hack
		//if(bEnableOrthographic){
			//DirectionUp = glm::vec3(1.0f, 0.0f, 0.0f); //TODO: remove this hack
			//DirectionUp = glm::vec3(0.0f, 0.0f, -1.0f); //TODO: remove this hack
		//}

		glm::vec3 Target = Position + DirectionFront;
		matrices.view = glm::lookAt(Position, Target, DirectionUp);

		//print for debug
		// if(bEnableOrthographic){
		// 	std::cout<<"Position="<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;
		// 	std::cout<<"Target="<<Target.x<<","<<Target.y<<","<<Target.z<<std::endl;
		// 	std::cout<<"DirectionUp="<<DirectionUp.x<<","<<DirectionUp.y<<","<<DirectionUp.z<<std::endl;
		// }



		//legacy code
		//if(cameraType == CameraType::FREE){ //FREE mode will focus on a place that is in front of the camera
			// glm::vec3 cameraPos = Position;
			// glm::vec3 cameraFront = DirectionFront;
			// glm::vec3 cameraUp = DirectionUp;
			// matrices.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//}
		// else if(cameraType == CameraType::LOCK){//LOCK: target position will be set to a object
		// 	glm::vec3 cameraPos = Position;
		// 	glm::vec3 cameraUp = DirectionUp;
		// 	matrices.view = glm::lookAt(cameraPos, TargetPosition, cameraUp);
		// }

		//matrices.view = TranslateMatrix * RotationMatrix;
		//matrices.view =  RotationMatrix * TranslateMatrix;

		//static unsigned count = 0;
		//if(count % 3000 == 0){
		//	std::cout<<"Camera Position="<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;
			//std::cout<<"Camera Rotation="<<Rotation.x<<","<<Rotation.y<<","<<Rotation.z<<std::endl;
		//}
		//count++;

		//std::cout<<"DirectionFront="<<DirectionFront.x<<","<<DirectionFront.y<<","<<DirectionFront.z<<std::endl;
		//std::cout<<"DirectionLeft="<<DirectionLeft.x<<","<<DirectionLeft.y<<","<<DirectionLeft.z<<std::endl;
		//std::cout<<"DirectionUp="<<DirectionUp.x<<","<<DirectionUp.y<<","<<DirectionUp.z<<std::endl;

		//std::cout<<"Velocity="<<Velocity.x<<","<<Velocity.y<<","<<Velocity.z<<std::endl;

		//glm::vec3 r = DirectionLeft * TempVelocity[LEFT].x;
		//std::cout<<"r="<<r.x<<","<<r.y<<","<<r.z<<std::endl;
		//std::cout<<"TempVelocity[LEFT].w="<<TempVelocity[LEFT].w<<std::endl;

		
		//glm::vec3 r;
		//r.x = RotationMatrix[0][0] * Position[0] +  RotationMatrix[0][1] * Position[1] +  RotationMatrix[0][2] * Position[2]; 
		//r.y = RotationMatrix[1][0] * Position[0] +  RotationMatrix[1][1] * Position[1] +  RotationMatrix[1][2] * Position[2]; 
		//r.z = RotationMatrix[2][0] * Position[0] +  RotationMatrix[2][1] * Position[1] +  RotationMatrix[2][2] * Position[2]; 
		//std::cout<<"r="<<r.x<<","<<r.y<<","<<r.z<<std::endl;

	};

	/* disable Game Pad for now
	// Update camera passing separate axis data (gamepad)
	// Returns true if view or position has been changed
	bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
	{
		bool retVal = false;

		if (type == CameraType::firstperson)
		{
			// Use the common console thumbstick layout		
			// Left = view, right = move

			const float deadZone = 0.0015f;
			const float range = 1.0f - deadZone;

			glm::vec3 camFront;
			camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
			camFront.y = sin(glm::radians(rotation.x));
			camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
			camFront = glm::normalize(camFront);

			float moveSpeed = deltaTime * movementSpeed;
			float rotSpeed = deltaTime * rotationSpeed;

			// Move
			if (fabsf(axisLeft.y) > deadZone)
			{
				float pos = (fabsf(axisLeft.y) - deadZone) / range;
				position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
				retVal = true;
			}
			if (fabsf(axisLeft.x) > deadZone)
			{
				float pos = (fabsf(axisLeft.x) - deadZone) / range;
				position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
				retVal = true;
			}

			// Rotate
			if (fabsf(axisRight.x) > deadZone)
			{
				float pos = (fabsf(axisRight.x) - deadZone) / range;
				rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
				retVal = true;
			}
			if (fabsf(axisRight.y) > deadZone)
			{
				float pos = (fabsf(axisRight.y) - deadZone) / range;
				rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
				retVal = true;
			}
		}
		else
		{
			// to do: move code from example base class for look-at
		}

		if (retVal)
		{
			updateViewMatrix();
		}

		return retVal;
	}
	*/
};

#endif