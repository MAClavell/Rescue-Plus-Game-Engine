#include "ResourceManager.h"
#include <sstream>

using namespace DirectX;

//Locks
static std::mutex texture2DLock;
static std::mutex cubemapLock;
static std::mutex meshLock;
static std::mutex pixelShaderLock;
static std::mutex vertexShaderLock;
static std::mutex contextLock;

struct TwoParamLoadData
{
	const char* address;
	ID3D11Device* device;
	TwoParamLoadData(const char* address, ID3D11Device* device)
		: address(address), device(device) { };
};
struct ThreeParamLoadData
{
	const char* address;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ThreeParamLoadData(const char* address, ID3D11Device* device, ID3D11DeviceContext* context)
		: address(address), device(device), context(context) { };
};

void ResourceManager::Release()
{
	//Delete Texture2Ds
	for (auto const& pair : texture2DMap)
	{
		if (pair.second) { pair.second->Release(); }
	}
	texture2DMap.clear();

	//Delete Cubemaps
	for (auto const& pair : cubemapMap)
	{
		if (pair.second) { pair.second->Release(); }
	}
	cubemapMap.clear();

	//Delete Meshes
	for (auto const& pair : meshMap)
	{
		if (pair.second) { delete pair.second; }
	}
	meshMap.clear();

	//Delete Pixel Shaders
	for (auto const& pair : pixelShaderMap)
	{
		if (pair.second) { delete pair.second; }
	}
	pixelShaderMap.clear();

	//Delete Vertex Shaders
	for (auto const& pair : vertexShaderMap)
	{
		if (pair.second) { delete pair.second; }
	}
	vertexShaderMap.clear();

	//Delete Materials
	for (auto const& pair : materialMap)
	{
		if (pair.second) { delete pair.second; }
	}
	materialMap.clear();

	//Delete Physics Materials
	for (auto const& pair : physicsMatMap)
	{
		if (pair.second) { delete pair.second; }
	}
	physicsMatMap.clear();
}

// Load a Texture2D from the specified address with MipMaps
bool ResourceManager::LoadTexture2D(const char* address, ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Create a string
	std::stringstream ss;
	ss << address;
	std::string str = ss.str();
	std::wstring lStr = std::wstring(str.begin(), str.end());
	const wchar_t* lAddress = lStr.c_str();

	//Check if the Texture2D is already in the map
	texture2DLock.lock();
	if (texture2DMap.find(str) != texture2DMap.end())
	{
		printf("Texture2D at address \"%s\" already exists in the resource manager\n", address);
		return false;
	}
	texture2DLock.unlock();

	//Load the Texture2D
	ID3D11ShaderResourceView* tex;
	//The context is not thread safe whe generating mipmaps
	contextLock.lock();
	if (CreateWICTextureFromFile(device, context, lAddress, 0, &tex) != S_OK)
	{
		printf("Could not load texture2D %s\n", address);
		return false;
	}
	contextLock.unlock();

	//Add to map
	texture2DLock.lock();
	texture2DMap.emplace(str, tex);
	texture2DLock.unlock();
	return true;
}

// Load a Texture2D from the specified address with NO MipMaps
bool ResourceManager::LoadTexture2D(const char* address, ID3D11Device * device)
{
	//Create a string
	std::stringstream ss;
	ss << address;
	std::string str = ss.str();
	std::wstring lStr = std::wstring(str.begin(), str.end());
	const wchar_t* lAddress = lStr.c_str();

	//Check if the Texture2D is already in the map
	texture2DLock.lock();
	if (texture2DMap.find(str) != texture2DMap.end())
	{
		printf("Texture2D at address \"%s\" already exists in the resource manager\n", address);
		return false;
	}
	texture2DLock.unlock();

	//Load the Texture2D
	ID3D11ShaderResourceView* tex;
	HRESULT res = CreateWICTextureFromFile(device, lAddress, 0, &tex);
	if (res != S_OK)
	{
		printf("Could not load Texture2D \"%s\"\n", address);
		return false;
	}

	//Add to map
	texture2DLock.lock();
	texture2DMap.emplace(str, tex);
	texture2DLock.unlock();
	return true;
}

// Load a Texture2D from the specified address asynchronously
static void LoadTexture2DAsyncHelperThree(Job* job, const void* userData)
{
	ThreeParamLoadData* data = *(ThreeParamLoadData**)(userData);
	ResourceManager::GetInstance()->LoadTexture2D(data->address, data->device, data->context);
	delete data;
}
static void LoadTexture2DAsyncHelperTwo(Job* job, const void* userData)
{
	TwoParamLoadData* data = *(TwoParamLoadData**)(userData);
	ResourceManager::GetInstance()->LoadTexture2D(data->address, data->device);
	delete data;
}
Job* ResourceManager::LoadTexture2DAsync(const char* address, ID3D11Device* device, Job* root)
{
	Job* job = nullptr;
	auto data = new TwoParamLoadData(address, device);
	if (root == nullptr)
		job = JobSystem::CreateJob(&LoadTexture2DAsyncHelperTwo, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadTexture2DAsyncHelperTwo, &data);
	JobSystem::Run(job);
	return job;
}
Job* ResourceManager::LoadTexture2DAsync(const char* address, ID3D11Device* device, ID3D11DeviceContext* context, Job* root)
{
	Job* job = nullptr;
	auto data = new ThreeParamLoadData(address, device, context);
	if (root == nullptr)
		job = JobSystem::CreateJob(&LoadTexture2DAsyncHelperThree, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadTexture2DAsyncHelperThree, &data);
	JobSystem::Run(job);
	return job;
}


// Load a CubeMap from the specified address with MipMaps
bool ResourceManager::LoadCubeMap(const char* address, ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Create a string
	std::stringstream ss;
	ss << address;
	std::string str = ss.str();
	std::wstring lStr = std::wstring(str.begin(), str.end());
	const wchar_t* lAddress = lStr.c_str();

	//Check if the CubeMap is already in the map
	cubemapLock.lock();
	if (cubemapMap.find(str) != cubemapMap.end())
	{
		printf("CubeMap at address \"%s\" already exists in the resource manager\n", address);
		return false;
	}
	cubemapLock.unlock();

	//Load the Texture2D
	ID3D11ShaderResourceView* tex;
	//The context is not thread safe whe generating mipmaps
	contextLock.lock();
	if(CreateDDSTextureFromFile(device, context, lAddress, 0, &tex) != S_OK)
	{
		printf("Could not load CubeMap \"%s\"\n", address);
		return false;
	}
	contextLock.unlock();

	//Add to map
	cubemapLock.lock();
	cubemapMap.emplace(str, tex);
	cubemapLock.unlock();
	return true;
}

// Load a CubeMap from the specified address with NO MipMaps
bool ResourceManager::LoadCubeMap(const char* address, ID3D11Device* device)
{
	//Create a string
	std::stringstream ss;
	ss << address;
	std::string str = ss.str();
	std::wstring lStr = std::wstring(str.begin(), str.end());
	const wchar_t* lAddress = lStr.c_str();

	//Check if the CubeMap is already in the map
	cubemapLock.lock();
	if (cubemapMap.find(str) != cubemapMap.end())
	{
		printf("CubeMap at address \"%s\" already exists in the resource manager\n", address);
		return false;
	}
	cubemapLock.unlock();

	//Load the Texture2D
	ID3D11ShaderResourceView* tex;
	if (CreateDDSTextureFromFile(device, lAddress, 0, &tex) != S_OK)
	{
		printf("Could not load CubeMap \"%s\"\n", address);
		return false;
	}

	//Add to map
	cubemapLock.lock();
	cubemapMap.emplace(str, tex);
	cubemapLock.unlock();
	return true;
}

// Load a CubeMap from the specified address asynchronously
static void LoadCubeMapAsyncHelperThree(Job* job, const void* userData)
{
	ThreeParamLoadData* data = *(ThreeParamLoadData**)(userData);
	ResourceManager::GetInstance()->LoadCubeMap(data->address, data->device, data->context);
	delete data;
}
static void LoadCubeMapAsyncHelperTwo(Job* job, const void* userData)
{
	TwoParamLoadData* data = *(TwoParamLoadData**)(userData);
	ResourceManager::GetInstance()->LoadCubeMap(data->address, data->device);
	delete data;
}
Job* ResourceManager::LoadCubeMapAsync(const char* address, ID3D11Device* device, Job* root)
{
	Job* job = nullptr;
	auto data = new TwoParamLoadData(address, device);
	if (root == nullptr)
		job = JobSystem::CreateJob(&LoadCubeMapAsyncHelperTwo, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadCubeMapAsyncHelperTwo, &data);
	JobSystem::Run(job);
	return job;
}
Job* ResourceManager::LoadCubeMapAsync(const char* address, ID3D11Device* device, ID3D11DeviceContext* context, Job* root)
{
	Job* job = nullptr;
	auto data = new ThreeParamLoadData(address, device, context);
	if (root == nullptr)
		job = JobSystem::CreateJob(&LoadCubeMapAsyncHelperThree, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadCubeMapAsyncHelperThree, &data);
	JobSystem::Run(job);
	return job;
}

// Load a Mesh from the specified address
bool ResourceManager::LoadMesh(const char* address, ID3D11Device* device)
{
	//Create a string
	std::stringstream ss;
	ss << address;
	std::string str = ss.str();

	//Check if the Mesh is already in the map
	meshLock.lock();
	if (meshMap.find(str) != meshMap.end())
	{
		printf("Mesh at address \"%s\" already exists in the resource manager\n", address);
		return false;
	}
	meshLock.unlock();

	//Load the Mesh
	Mesh* mesh = new Mesh(address, device);
	if (!mesh->IsMeshLoaded())
	{
		printf("Could not load Mesh \"%s\"\n", address);
		if (mesh) { delete mesh; }
		return false;
	}

	//Add to map
	meshLock.lock();
	meshMap.emplace(str, mesh);
	meshLock.unlock();
	return true;
}

// Load a Mesh from the specified address asynchronously
static void LoadMeshAsyncHelper(Job* job, const void* userData)
{
	TwoParamLoadData* data = *(TwoParamLoadData**)(userData);
	ResourceManager::GetInstance()->LoadMesh(data->address, data->device);
	delete data;
}
Job* ResourceManager::LoadMeshAsync(const char* address, ID3D11Device* device, Job* root)
{
	Job* job = nullptr;
	auto data = new TwoParamLoadData(address, device);
	if(root == nullptr)
		job = JobSystem::CreateJob(&LoadMeshAsyncHelper, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadMeshAsyncHelper, &data);
	JobSystem::Run(job);
	return job;
}

// Load a Material from the specified address
bool ResourceManager::AddMaterial(const char* name, Material* material)
{
	//Create a string
	std::stringstream ss;
	ss << name;
	std::string str = ss.str();

	//Check if the Material is already in the map
	if (materialMap.find(str) != materialMap.end())
	{
		printf("Material of name \"%s\" already exists in the resource manager\n", name);
		return false;
	}
	
	//Add to map
	materialMap.emplace(str, material);
	return true;
}

// Load a Pixel Shader from the specified address
bool ResourceManager::LoadPixelShader(const char* name, ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Create a string
	std::stringstream ss;
	ss << name;
	std::string str = ss.str();
	std::wstring lStr = std::wstring(str.begin(), str.end());
	const wchar_t* lName = lStr.c_str();

	//Check if the Pixel Shader is already in the map
	pixelShaderLock.lock();
	if (pixelShaderMap.find(str) != pixelShaderMap.end())
	{
		printf("Pixel Shader of name \"%s\" already exists in the resource manager\n", name);
		return false;
	}
	pixelShaderLock.unlock();

	//Load shader
	SimplePixelShader* ps = new SimplePixelShader(device, context);
	if (!ps->LoadShaderFile(lName))
	{
		printf("Could not load Pixel Shader \"%s\"\n", name);
		return false;
	}

	//Add to map
	pixelShaderLock.lock();
	pixelShaderMap.emplace(str, ps);
	pixelShaderLock.unlock();
	return false;
}

// Load a Pixel Shader from the specified address asynchronously
static void LoadPixelShaderAsyncHelper(Job* job, const void* userData)
{
	ThreeParamLoadData* data = *(ThreeParamLoadData**)(userData);
	ResourceManager::GetInstance()->LoadPixelShader(data->address, data->device, data->context);
	delete data;
}
Job* ResourceManager::LoadPixelShaderAsync(const char* address, ID3D11Device* device, ID3D11DeviceContext* context, Job* root)
{
	Job* job = nullptr;
	auto data = new ThreeParamLoadData(address, device, context);
	if (root == nullptr)
		job = JobSystem::CreateJob(&LoadPixelShaderAsyncHelper, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadPixelShaderAsyncHelper, &data);
	JobSystem::Run(job);
	return job;
}

// Load a Vertex Shader from the specified address
bool ResourceManager::LoadVertexShader(const char* name, ID3D11Device* device, ID3D11DeviceContext* context)
{
	//Create a string
	std::stringstream ss;
	ss << name;
	std::string str = ss.str();
	std::wstring lStr = std::wstring(str.begin(), str.end());
	const wchar_t* lName = lStr.c_str();

	//Check if the Vertex Shader is already in the map
	vertexShaderLock.lock();
	if (vertexShaderMap.find(str) != vertexShaderMap.end())
	{
		printf("Vertex Shader of name \"%s\" already exists in the resource manager\n", name);
		return false;
	}
	vertexShaderLock.unlock();

	//Load shader
	SimpleVertexShader* vs = new SimpleVertexShader(device, context);
	if (!vs->LoadShaderFile(lName))
	{
		printf("Could not load Vertex Shader \"%s\"\n", name);
		return false;
	}

	//Add to map
	vertexShaderLock.lock();
	vertexShaderMap.emplace(str, vs);
	vertexShaderLock.unlock();
	return false;
}

// Load a Vertex Shader from the specified address asynchronously
static void LoadVertexShaderAsyncHelper(Job* job, const void* userData)
{
	ThreeParamLoadData* data = *(ThreeParamLoadData * *)(userData);
	ResourceManager::GetInstance()->LoadVertexShader(data->address, data->device, data->context);
	delete data;
}
Job* ResourceManager::LoadVertexShaderAsync(const char* address, ID3D11Device* device, ID3D11DeviceContext* context, Job* root)
{
	Job* job = nullptr;
	auto data = new ThreeParamLoadData(address, device, context);
	if (root == nullptr)
		job = JobSystem::CreateJob(&LoadVertexShaderAsyncHelper, &data);
	else job = JobSystem::CreateJobAsChild(root, &LoadVertexShaderAsyncHelper, &data);
	JobSystem::Run(job);
	return job;
}

// Add an existing Physics Material to the manager
bool ResourceManager::AddPhysicsMaterial(const char* name, PhysicsMaterial* material)
{
	//Create a string
	std::stringstream ss;
	ss << name;
	std::string str = ss.str();

	//Check if the Physics Material is already in the map
	if (materialMap.find(str) != materialMap.end())
	{
		printf("Physics Material of name \"%s\" already exists in the resource manager\n", name);
		return false;
	}

	//Add to map
	physicsMatMap.emplace(str, material);
	return true;
}

// Get a loaded Texture2D
ID3D11ShaderResourceView* ResourceManager::GetTexture2D(std::string address)
{
	//Check if the Texture2D is in the map
	if (texture2DMap.find(address) == texture2DMap.end())
	{
		printf("Texture2D at address \"%s\" does not exist in the resource manager\n", address.c_str());
		return nullptr;
	}

	return texture2DMap[address];
}

// Get a loaded CubeMap
ID3D11ShaderResourceView* ResourceManager::GetCubeMap(std::string address)
{
	//Check if the CubeMap is in the map
	if (cubemapMap.find(address) == cubemapMap.end())
	{
		printf("CubeMap at address \"%s\" does not exist in the resource manager\n", address.c_str());
		return nullptr;
	}

	return cubemapMap[address];
}

// Get a loaded Mesh
Mesh* ResourceManager::GetMesh(std::string address)
{
	//Check if the Mesh is in the map
	if (meshMap.find(address) == meshMap.end())
	{
		printf("Mesh at address \"%s\" does not exist in the resource manager\n", address.c_str());
		return nullptr;
	}

	return meshMap[address];
}

// Get a added Material
Material* ResourceManager::GetMaterial(std::string name)
{
	//Check if the Material is in the map
	if (materialMap.find(name) == materialMap.end())
	{
		printf("Material of name \"%s\" does not exist in the resource manager\n", name.c_str());
		return nullptr;
	}

	return materialMap[name];
}

// Get a loaded Pixel Shader
SimplePixelShader* ResourceManager::GetPixelShader(std::string name)
{
	//Check if the Pixel Shader is in the map
	if (pixelShaderMap.find(name) == pixelShaderMap.end())
	{
		printf("Pixel Shader of name \"%s\" does not exist in the resource manager\n", name.c_str());
		return nullptr;
	}

	return pixelShaderMap[name];
}

// Get a loaded Vertex Shader
SimpleVertexShader* ResourceManager::GetVertexShader(std::string name)
{
	//Check if the Vertex Shader is in the map
	if (vertexShaderMap.find(name) == vertexShaderMap.end())
	{
		printf("Vertex Shader of name \"%s\" does not exist in the resource manager\n", name.c_str());
		return nullptr;
	}

	return vertexShaderMap[name];
}

PhysicsMaterial* ResourceManager::GetPhysicsMaterial(std::string name)
{
	//Check if the Physics Material is in the map
	if (physicsMatMap.find(name) == physicsMatMap.end())
	{
		printf("Physics Material of name \"%s\" does not exist in the resource manager\n", name.c_str());
		return nullptr;
	}

	return physicsMatMap[name];
}
