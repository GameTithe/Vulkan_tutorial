#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Vertex.h"

class ObjLoader
{
public:
	static void loadModel(std::string MODEL_PATH, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
		{
			throw std::runtime_error(warn + err);
		}


		// pos 범위 계산
		float maxX = FLT_MIN, minX = FLT_MAX, maxY = FLT_MIN, minY = FLT_MAX, maxZ = FLT_MIN, minZ = FLT_MAX;

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				float x = attrib.vertices[3 * index.vertex_index + 0];
				float y = attrib.vertices[3 * index.vertex_index + 1];
				float z = attrib.vertices[3 * index.vertex_index + 2];

				maxX = x > maxX ? x : maxX;
				minX = x < minX ? x : minX;

				maxY = y > maxY ? y : maxY;
				minY = y < minY ? y : minY;

				maxZ = z > maxZ ? z : maxZ;
				minZ = z < minZ ? z : minZ;
			}
		}

		float extentX = (maxX - minX);
		float extentY = (maxY - minY);
		float extentZ = (maxZ - minZ);

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);


			}
		}

		float resize = 2.0f / (std::max({ extentX, extentY, extentZ, 0.001f }));

		float centerX = (maxX + minX) * 0.5f, centerY = (maxY + minY) * 0.5f, centerZ = (maxZ + minZ) * 0.5f;

		// resacaling, repositioning
		for (auto& vertex : vertices)
		{
			vertex.pos.x = (vertex.pos.x - centerX) * resize;
			vertex.pos.y = (vertex.pos.y - centerY) * resize;
			vertex.pos.z = (vertex.pos.z - centerZ) * resize;
		}
		// normal vector	정규화 
	}

};