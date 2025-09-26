#pragma once

#include "Mesh.h"
#include "Texture.h"

class Modelo2
{
public:
	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();

private:

	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::vector<Mesho2*> meshList;
	std::vector<TextureO2*> textureList;
	std::vector<unsigned int> meshToTex;
};
