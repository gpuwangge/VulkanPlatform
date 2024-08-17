#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CApplication{
	//a blue triangle on the screen
	CObject object;

	std::string vertexShader = "simpleTriangle/vert.spv";
	std::string fragmentShader = "simpleTriangle/frag.spv";

public:
	void initialize(){
		CSupervisor::Register((CApplication*)this);
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		CSupervisor::Activate_Pipeline(); 
		
		object.Register((CApplication*)this, -1, -1, 0); //no texture, no model, id=0
		
		CApplication::initialize();
	}

	void update(){
		//static int counter = 1;
		CApplication::update();

		//if(counter==7504) NeedToExit = true;
		//counter++;
	}

	void recordGraphicsCommandBuffer(){
		object.Draw(3);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

