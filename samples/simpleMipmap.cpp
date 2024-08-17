#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CSimpleMipmap
class TEST_CLASS_NAME: public CApplication{
public:
	CObject object;

	std::vector<std::string> modelNames = {"hallway.obj"}; //"viking_room.obj"
	std::vector<std::pair<std::string, bool>> textureNames = {{"checkerboard_marble.jpg", true}}; //first: textureName, second: mipmap
	std::string vertexShader = "simpleMipmap/vert.spv";
	std::string fragmentShader = "simpleMipmap/frag.spv";	
	
    void initialize(){
		mainCamera.type = Camera::CameraType::firstperson;
		mainCamera.setPosition(glm::vec3(0.0f, -0.8f, 0.0f));
		mainCamera.setRotation(glm::vec3(0.0f, 90.00001f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		CSupervisor::Register((CApplication*)this);
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		CSupervisor::Activate_Uniform_Graphics_MVP();
		CSupervisor::Activate_Uniform_Graphics_Sampler();
		CSupervisor::Activate_Feature_Graphics_MSAA();
		CSupervisor::Activate_Feature_Graphics_RainbowMipmap();
		CSupervisor::Activate_Buffer_Graphics_Vertex(modelNames);
		CSupervisor::Activate_Texture(&textureNames);
		CSupervisor::Activate_Pipeline();

		object.Register((CApplication*)this);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif