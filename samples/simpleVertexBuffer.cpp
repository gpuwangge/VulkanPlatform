#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleVertexBuffer
class TEST_CLASS_NAME: public CApplication{
//a triangle on the screen, each vertex has R, G or B color. Fragment color is interpolated
public:
	std::vector<Vertex2D> vertices = {
		{ { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
		{ { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
	};

	std::vector<int> modelList = {0}; //this 0 means vertex3D, not model

	void initialize(){
		appInfo.Object.Count = 1;
		appInfo.Buffer.GraphicsVertex.Vertices2D = &vertices; 
		appInfo.Object.Model.List = &modelList;
		appInfo.Shader.Vertex = "simpleVertexBuffer/vert.spv";
		appInfo.Shader.Fragment = "simpleVertexBuffer/frag.spv";
		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		objectList[0].Draw(3);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

