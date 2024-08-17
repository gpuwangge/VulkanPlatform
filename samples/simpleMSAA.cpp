#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CSimpleMSAA
class TEST_CLASS_NAME: public CApplication{
public:
	CObject object;

	std::vector<std::string> modelNames = {"viking_room.obj"}; 
	std::vector<std::pair<std::string, bool>> textureNames = {{"viking_room.png", false}}; //first: textureName, second: mipmap
	std::string vertexShader = "simpleMSAA/vert.spv";
	std::string fragmentShader = "simpleMSAA/frag.spv";	

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		CSupervisor::Register((CApplication*)this);
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		CSupervisor::Activate_Uniform_Graphics_MVP();
		CSupervisor::Activate_Feature_Graphics_MSAA();
		CSupervisor::Activate_Uniform_Graphics_Sampler();
		CSupervisor::Activate_Buffer_Graphics_Vertex(modelNames);
		CSupervisor::Activate_Texture(&textureNames);
		CSupervisor::Activate_Pipeline();
	
		object.Register((CApplication*)this);

		CApplication::initialize();
	}

	void update(){
		//CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		object.SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif