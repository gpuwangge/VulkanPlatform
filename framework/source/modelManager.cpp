#include "../include/modelManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../thirdParty/tiny_obj_loader.h"

CModelManager::CModelManager(){}
CModelManager::~CModelManager(){}


void CModelManager::LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

#ifndef ANDROID
	std::string fullModelPath = MODEL_PATH + modelName;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullModelPath.c_str())) {
		fullModelPath = "models/" + modelName;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullModelPath.c_str())) 
			throw std::runtime_error(warn + err);
	}
		
#else
	std::vector<uint8_t> fileBits;
	std::string fullModelPath = ANDROID_MODEL_PATH + modelName;
	CContext::GetHandle().androidManager.AssetReadFile(fullModelPath.c_str(), fileBits);
	std::istringstream in_stream(std::string(fileBits.begin(), fileBits.end()));
   	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &in_stream))
		throw std::runtime_error(warn + err);
#endif
	
	//if use unordered_map, need to implement size_t operator()(Vertex3D const& vertex) const. 
	//for some reason(c++ 11 stl not recognized in GLM) android version doesn't support <glm/gtx/hash.hpp>
	//implment custom hash function instead, checkout dataBuffer.hpp
	std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};

	//vertices3D.clear();
	//indices3D.clear();
	
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex3D vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices3D.size());
				vertices3D.push_back(vertex);
			}

			indices3D.push_back(uniqueVertices[vertex]);
		}
	}
}




