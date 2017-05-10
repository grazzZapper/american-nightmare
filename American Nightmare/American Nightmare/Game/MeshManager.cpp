#include "MeshManager.h"

MeshManager::MeshManager() { }

MeshManager::MeshManager(const MeshManager& other) { }

MeshManager::~MeshManager() { Clear(); }

void MeshManager::Clear()
{
	for (int i = 0; i < meshList.size(); i++)
	{
		meshList[i]->model->Stop();
		delete meshList[i];
	}
	meshList.clear();
}

bool MeshManager::AddMesh(std::string name, int nrOfVertices, std::vector<AVertex> vertices)
{
	// Checking if mesh is already added
//	if (getMesh(name) != nullptr)
//		return false;

	// Making model from file
	Model* model;
	model = new Model();
	if (model == nullptr) return false;

	// Making AVertex into a VertexUV
	std::vector<VertexUV> vertexUV;
	for (int i = 0; i < vertices.size(); i++)
	{
		VertexUV outV;
		AVertex inV = vertices[i];

		// Position
		outV.x = inV.position[0];
		outV.y = inV.position[1];
		outV.z = inV.position[2];

		// Normals
		outV.nx = inV.normal[0];
		outV.ny = inV.normal[1];
		outV.nz = inV.normal[2];

		// Tex Coords
		outV.u = inV.uv[0];
		outV.v = inV.uv[1];

		// Pushing into our new vector
		vertexUV.push_back(outV);

		printf("vertex pos: %f, %f, %f\n", outV.x, outV.y, outV.z);

	}

	// Making model out of vertices
	if (!model->LoadModel(vertexUV))
		return false;

	// Make model into mesh
	Mesh* mesh;
	mesh = new Mesh(name, model);
	
	// Adding mesh to list
	meshList.push_back(mesh);

	return true;
}

MeshManager::Mesh* MeshManager::getMesh(std::string name) const
{
	Mesh* outMesh = nullptr;

	// Finding the correct mesh, returns nullptr if not found
	for (Mesh* mesh : meshList)
	{
		if (mesh->name == name)
		{
			outMesh = mesh;
			break;
		}
	}

	// Could not find mesh
	if (outMesh == nullptr)
		printf("Could not find mesh: %s\n", name.c_str());

	return outMesh;
}

std::vector<MeshManager::Mesh*> MeshManager::getMeshList() const
{
	return meshList;
}


