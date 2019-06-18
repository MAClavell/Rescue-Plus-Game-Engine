#include "Renderer.h"
#include <sstream> 

// For the DirectX Math library
using namespace DirectX;

// Constructor - Set up the MeshRenderer
MeshRenderer::MeshRenderer(GameObject* gameObject, Mesh* mesh, Material* material)
	: Component::Component(gameObject)
{
	this->mesh = mesh;
	this->material = material;

	//Create a unique identifer (combination of the two addresses).
	//	Used in the renderer
	const void * addressMat = static_cast<const void*>(material);
	const void * addressMesh = static_cast<const void*>(mesh);
	std::stringstream ss;
	ss << addressMat << addressMesh;
	std::string temp = ss.str();
	identifier = ss.str();

	Renderer::GetInstance()->AddMeshRenderer(this);
}

// Destructor for when an instance is deleted
MeshRenderer::~MeshRenderer()
{ 
	Renderer::GetInstance()->RemoveMeshRenderer(this);
}

// Get the material this MeshRenderer uses
Material* MeshRenderer::GetMaterial()
{
	return material;
}

// Get the mesh this MeshRenderer uses
Mesh* MeshRenderer::GetMesh()
{
	return mesh;
}

// Get the material/mesh identifier
std::string MeshRenderer::GetMatMeshIdentifier()
{
	return identifier;
}