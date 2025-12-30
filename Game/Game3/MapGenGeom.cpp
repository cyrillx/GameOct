#include "stdafx.h"
#include "Map.h"
//=============================================================================
auto getCorner(const glm::vec3& center, float width, float height, float depth)
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float halfDepth = depth / 2.0f;

	// Define the 8 corner points relative to the center
	glm::vec3 p0 = center + glm::vec3(-halfWidth, -halfHeight, -halfDepth); // 0: Left-Bottom-Front
	glm::vec3 p1 = center + glm::vec3(halfWidth, -halfHeight, -halfDepth);  // 1: Right-Bottom-Front
	glm::vec3 p2 = center + glm::vec3(halfWidth, halfHeight, -halfDepth);   // 2: Right-Top-Front
	glm::vec3 p3 = center + glm::vec3(-halfWidth, halfHeight, -halfDepth);  // 3: Left-Top-Front

	glm::vec3 p4 = center + glm::vec3(-halfWidth, -halfHeight, halfDepth);  // 4: Left-Bottom-Back
	glm::vec3 p5 = center + glm::vec3(halfWidth, -halfHeight, halfDepth);   // 5: Right-Bottom-Back
	glm::vec3 p6 = center + glm::vec3(halfWidth, halfHeight, halfDepth);    // 6: Right-Top-Back
	glm::vec3 p7 = center + glm::vec3(-halfWidth, halfHeight, halfDepth);   // 7: Left-Top-Back

	return std::array<glm::vec3, 8>{ p0, p1, p2, p3, p4, p5, p6, p7 };
}
//=============================================================================
void AddBox(const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor, bool enablePlane[6])
{
	auto p = getCorner(center, width, height, depth);

	// Face Colors (optional, for visual distinction)
#if 0 // DEBUG
	glm::vec3 red(1.0f, 0.0f, 0.0f);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glm::vec3 blue(0.0f, 0.0f, 1.0f);
	glm::vec3 yellow(1.0f, 1.0f, 0.0f);
	glm::vec3 cyan(0.0f, 1.0f, 1.0f);
	glm::vec3 magenta(1.0f, 0.0f, 1.0f);
#else
	const glm::vec3& red = color;
	const glm::vec3& green = color;
	const glm::vec3& blue = color;
	const glm::vec3& yellow = color;
	const glm::vec3& cyan = color;
	const glm::vec3& magenta = color;
#endif

	// Front Face Z
	GeometryGenerator::AddPlane(p[0], p[3], p[2], p[1], verticesWall, indicesWall, blue, glm::vec2(0, 0), glm::vec2(0, height), glm::vec2(width, height), glm::vec2(width, 0));
	// Back Face Z
	GeometryGenerator::AddPlane(p[5], p[6], p[7], p[4], verticesWall, indicesWall, red, glm::vec2(width, 0), glm::vec2(width, height), glm::vec2(0, height), glm::vec2(0, 0));
	// Top Face Y
	GeometryGenerator::AddPlane(p[3], p[7], p[6], p[2], verticesFloor, indicesFloor, green, glm::vec2(0, width), glm::vec2(0, 0), glm::vec2(depth, 0), glm::vec2(depth, width));
	// Bottom Face Y
	GeometryGenerator::AddPlane(p[4], p[0], p[1], p[5], verticesCeil, indicesCeil, yellow, glm::vec2(0, depth), glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(width, depth));
	// Right Face X
	GeometryGenerator::AddPlane(p[1], p[2], p[6], p[5], verticesWall, indicesWall, cyan, glm::vec2(0, 0), glm::vec2(0, height), glm::vec2(depth, height), glm::vec2(depth, 0));
	// Left Face X 
	GeometryGenerator::AddPlane(p[4], p[7], p[3], p[0], verticesWall, indicesWall, magenta, glm::vec2(0, 0), glm::vec2(0, height), glm::vec2(depth, height), glm::vec2(depth, 0));
}
//=============================================================================