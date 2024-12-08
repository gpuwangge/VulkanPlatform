#ifndef H_CAMERA
#define H_CAMERA

#include "entity.h"

class Camera : public CEntity
{
private:
	float fov;
	float znear, zfar;

	/* legacy code
	void updateViewMatrix()
	{
		glm::mat4 rotM = glm::mat4(1.0f);
		glm::mat4 transM;

		rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 translation = position;
		if (flipY) {
			translation.y *= -1.0f;
		}
		transM = glm::translate(glm::mat4(1.0f), translation);

		if (type == CameraType::firstperson)
		{
			matrices.view = rotM * transM;
		}
		else
		{
			matrices.view = transM * rotM;
		}

		viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

		updated = true;
	};*/
public:
	enum CameraType { lookat, freemove };
	CameraType cameraType = CameraType::lookat;

	glm::vec3 TargetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	void SetTargetPosition(float x, float y, float z){ TargetPosition = glm::vec3(x, y, z); }
	//glm::vec3 rotation = glm::vec3();
	//glm::vec3 position = glm::vec3();
	//glm::vec4 viewPos = glm::vec4();
	//float rotationSpeed = 1.0f;
	//float movementSpeed = 1.0f;
	//bool updated = false;

	//bool flipY = false;

	Camera(){
		entityType = EntityType::camera;
		setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
	}

	struct{
		glm::mat4 perspective;
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

	// bool active(){
	// 	return state.left || state.right || state.up || state.down || state.forward || state.backward 
	// 		|| state.pitchup || state.pitchdown || state.yawleft || state.yawright || state.rollleft || state.rollright;
	// }

	//float getNearClip() {  return znear; }
	//float getFarClip() { return zfar;}

	//this function is provided to user
	void setPerspective(float fov, float aspect, float znear, float zfar){ 
		this->fov = fov;
		this->znear = znear;
		this->zfar = zfar;
		matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		//if (flipY) matrices.perspective[1][1] *= -1.0f;
	};

	//this function is provided to user (no use case yet)
	void updateAspectRatio(float aspect){
		matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		//if (flipY) matrices.perspective[1][1] *= -1.0f;
	}

	/* legacy code
	void setPosition(glm::vec3 position)
	{
		this->position = position;
		updateViewMatrix();
	}

	void setRotation(glm::vec3 rotation)
	{
		this->rotation = rotation;
		updateViewMatrix();
	}

	void rotate(glm::vec3 delta)
	{
		this->rotation += delta;
		updateViewMatrix();
	}

	void setTranslation(glm::vec3 translation)
	{
		this->position = translation;
		updateViewMatrix();
	};

	void translate(glm::vec3 delta)
	{
		this->position += delta;
		updateViewMatrix();
	}

	void setRotationSpeed(float rotationSpeed)
	{
		this->rotationSpeed = rotationSpeed;
	}

	void setMovementSpeed(float movementSpeed)
	{
		this->movementSpeed = movementSpeed;
	}*/

	void update(float deltaTime){
		CEntity::Update(deltaTime); //update TranslateMatrix RotationMatrix ScaleMatrix
		//matrices.view = TranslateMatrix * RotationMatrix;
		//matrices.view =  RotationMatrix * TranslateMatrix;

		static unsigned count = 0;
		if(count % 1000 == 0){
			std::cout<<"Camera Position="<<Position.x<<","<<Position.y<<","<<Position.z<<std::endl;
			//std::cout<<"Camera Rotation="<<Rotation.x<<","<<Rotation.y<<","<<Rotation.z<<std::endl;
		}
		count++;

		
		if(cameraType == CameraType::freemove){
			glm::vec3 cameraPos = Position;//glm::vec3(0.0f, 0.0f, 3.0f);
			glm::vec3 cameraFront = DirectionFront;//glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 cameraUp = DirectionUp;// glm::vec3(0.0f, 1.0f, 0.0f);
			matrices.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}else if(cameraType == CameraType::lookat){
			glm::vec3 cameraPos = Position;//glm::vec3(0.0f, 0.0f, 3.0f);
			//glm::vec3 cameraFront = DirectionFront;//glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 cameraUp = DirectionUp;// glm::vec3(0.0f, 1.0f, 0.0f);
			matrices.view = glm::lookAt(cameraPos, TargetPosition, cameraUp);//TODO
		}

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


		// if (active()){
		// 	if(state.forward);
		// 	if(state.backward);
		// 	if(state.left);
		// 	if(state.right);
		// 	if(state.up);
		// 	if(state.down);
		// 	if(state.pitchup);
		// 	if(state.pitchdown);
		// 	if(state.yawleft);
		// 	if(state.yawright);
		// 	if(state.rollleft);
		// 	if(state.rollright);
		// }

		/* legacy code
		updated = false;
		if (type == CameraType::firstperson)
		{
			if (moving())
			{
				glm::vec3 camFront;
				camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
				camFront.y = sin(glm::radians(rotation.x));
				camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
				camFront = glm::normalize(camFront);

				float moveSpeed = deltaTime * movementSpeed;
				float rotSpeed = deltaTime * rotationSpeed;

				if (keys.up)//TODO: debug
					position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 0.0f, 1.0f))) * moveSpeed;
				if (keys.down)//TODO: debug
					position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 0.0f, 1.0f))) * moveSpeed;
				if (keys.left)
					position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
				if (keys.right)
					position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
				if (keys.forward)
					position += camFront * moveSpeed;
				if (keys.backward)
					position -= camFront * moveSpeed;
				if (keys.turnLeft)
					rotation -= glm::vec3(0.0f, 1.0f, 0.0f) * rotSpeed;
				if (keys.turnRight)
					rotation += glm::vec3(0.0f, 1.0f, 0.0f) * rotSpeed;
			}
		}
		updateViewMatrix();*/
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