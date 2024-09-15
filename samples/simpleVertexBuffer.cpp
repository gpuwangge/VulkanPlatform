#include "..\\framework\\include\\application.h"
#include "object.h"
#define TEST_CLASS_NAME CSimpleVertexBuffer
class TEST_CLASS_NAME: public CApplication{
//a triangle on the screen, each vertex has R, G or B color. Fragment color is interpolated
public:
	std::vector<Vertex2D> vertices = {
		{ { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
		{ { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
	};

	CObject object;

	std::string vertexShader = "simpleVertexBuffer/vert.spv";
	std::string fragmentShader = "simpleVertexBuffer/frag.spv";

	void initialize(){
		CSupervisor::VertexShader = vertexShader;
		CSupervisor::FragmentShader = fragmentShader;
		Activate_Buffer_Graphics_Vertex(vertices);
		Activate_Pipeline();

		object.Register((CApplication*)this, -1, 0, 0); //no texture, no model, id=0

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw(3);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

