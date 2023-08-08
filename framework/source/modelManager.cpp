#include "modelManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

CModelManager::CModelManager(){}
CModelManager::~CModelManager(){}


void CModelManager::LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelName.c_str())) {
		throw std::runtime_error(warn + err);
	}

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

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices3D.size());
				vertices3D.push_back(vertex);
			}

			indices3D.push_back(uniqueVertices[vertex]);
		}
	}
}




