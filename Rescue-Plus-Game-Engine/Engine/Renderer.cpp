#include "Renderer.h"
#include "LightManager.h"
#include "ResourceManager.h"
#include "ExtendedMath.h"

using namespace DirectX;

// Initialize values in the renderer
void Renderer::Init(ID3D11Device* device, ID3D11DeviceContext* context, UINT width, UINT height)
{
	// Assign default clear color
	this->SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// --------------------------------------------------------
	//Get skybox information
	skyboxMat = ResourceManager::GetInstance()->GetMaterial("skybox");
	cubeMesh = ResourceManager::GetInstance()->GetMesh("Assets\\Models\\Basic\\cube.obj");

	D3D11_RASTERIZER_DESC skyRD = {};
	skyRD.CullMode = D3D11_CULL_FRONT;
	skyRD.FillMode = D3D11_FILL_SOLID;
	skyRD.DepthClipEnable = true;
	device->CreateRasterizerState(&skyRD, &skyRasterState);

	D3D11_DEPTH_STENCIL_DESC skyDS = {};
	skyDS.DepthEnable = true;
	skyDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	skyDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&skyDS, &skyDepthState);


	// --------------------------------------------------------
	//Create batch for debug drawing
	db_states = std::make_unique<CommonStates>(device);
	db_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	db_effect = std::make_unique<BasicEffect>(device);
	db_effect->SetVertexColorEnabled(true);

	{
		void const* shaderByteCode;
		size_t byteCodeLength;

		db_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		device->CreateInputLayout(
			VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			db_inputLayout.ReleaseAndGetAddressOf());
	}


	// --------------------------------------------------------
	//Set states for transparency

	// Depth state
	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&ds, &transparentDepthState);

	//blend
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false; 
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	// These control how the RGB channels are combined
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	// These control how the alpha channel is combined
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bd, &transparentBlendState);


	// --------------------------------------------------------
	//Get shadow information
	shadowVS = ResourceManager::GetInstance()->GetVertexShader("VS_Shadow.cso");

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);
}

// Destructor for when the singleton instance is deleted
void Renderer::Release()
{
	//Clean up skybox
	skyDepthState->Release();
	skyRasterState->Release();

	//Clean up transparency
	transparentBlendState->Release();
	transparentDepthState->Release();

	//Clean up shadow map
	shadowRasterizer->Release();
}

// Draw all entities in the render list
void Renderer::Draw(ID3D11DeviceContext* context, 
					ID3D11Device* device,
					Camera* camera,
					ID3D11RenderTargetView* backBufferRTV,
					ID3D11DepthStencilView* depthStencilView,
					ID3D11SamplerState* sampler,
					UINT width, UINT height, float deltaTime)
{
	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, this->clearColor);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	RenderShadowMaps(context, device, camera, backBufferRTV, depthStencilView, width, height);

	DrawOpaqueObjects(context, camera);

	DrawSky(context, camera);
	
	DrawDebugShapes(context, camera, deltaTime);

	DrawTransparentObjects(context, camera);

	// Need to unbind the shadow map from pixel shader stage
	// so it can be rendered into properly next frame
	// (Just unbinding all since we don't know which register its in)
	ID3D11ShaderResourceView* nullSRVs[16] = {};
	context->PSSetShaderResources(0, 16, nullSRVs);
}

// Render shadow maps for all lights that cast shadows
void Renderer::RenderShadowMaps(ID3D11DeviceContext* context,
	ID3D11Device* device,
	Camera* camera,
	ID3D11RenderTargetView* backBufferRTV,
	ID3D11DepthStencilView* depthStencilView,
	UINT width, UINT height)
{
	std::vector<Light*> lights = LightManager::GetInstance()->GetShadowCastingLights();
	context->RSSetState(shadowRasterizer);
	context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

	// SET A VIEWPORT!!!
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)SHADOW_MAP_SIZE;
	vp.Height = (float)SHADOW_MAP_SIZE;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	//Loop through all lights that cast shadows and draw to their textures
	for (auto l : lights)
	{
		//Create shadow SRV if it does not exist
		if (l->GetShadowSRV() == nullptr)
			l->InitShadowMap(device);

		ID3D11DepthStencilView* shadowDSV = l->GetShadowDSV();
		ID3D11ShaderResourceView* shadowSRV = l->GetShadowSRV();

		// Initial setup - No RTV necessary - Clear shadow map
		context->OMSetRenderTargets(0, 0, shadowDSV);
		context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// Set up the shaders
		shadowVS->SetShader();
		shadowVS->SetMatrix4x4("view", l->GetViewMatrix());
		shadowVS->SetMatrix4x4("projection", l->GetProjectionMatrix());
		shadowVS->CopyBufferData("once");

		//Loop through entities (simplified. Look at DrawOpaqueObjects for better documentation)
		for (auto const& mapPair : renderMap)
		{
			if (mapPair.second.size() < 1)
				return;

			std::vector<MeshRenderer*> list = mapPair.second;

			Mesh* mesh = list[0]->GetMesh();

			// Set buffers in the input assembler
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			//Loop through each entity in the list
			for (size_t i = 0; i < list.size(); i++)
			{
				//Don't draw disabled entities
				if (!list[i]->gameObject()->GetEnabled())
					continue;

				shadowVS->SetMatrix4x4("world", list[i]->gameObject()->GetWorldMatrix());
				shadowVS->CopyBufferData("perObject");

				// Finally do the actual drawing
				context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
			}
		}
	}

	// Revert to original pipeline state
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	vp.Width = (float)width;
	vp.Height = (float)height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0);
}

// Draw opaque objects
void Renderer::DrawOpaqueObjects(ID3D11DeviceContext* context, Camera* camera)
{
	//TODO: Apply attenuation
	//context->OMSetDepthStencilState(waterDepthState, 0);
	for (auto const& mapPair : renderMap)
	{
		if (mapPair.second.size() < 1)
			return;

		//Get list, material, and mesh
		std::vector<MeshRenderer*> list = mapPair.second;

		Material* mat = list[0]->GetMaterial();
		Mesh* mesh = list[0]->GetMesh();

		//Since materials are shared, if the alpha is below 1 add them all to the transparency list
		if (mat->GetAlpha() < 1)
		{
			for (size_t i = 0; i < list.size(); i++)
			{
				transparentObjList.push_back(list[0]);
			}
			continue;
		}

		// Turn shaders on
		mat->GetVertexShader()->SetShader();
		mat->GetPixelShader()->SetShader();

		//Prepare the material's combo specific variables
		mat->PrepareMaterialCombo(list[0]->gameObject(), camera);

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Loop through each entity in the list
		for (size_t i = 0; i < list.size(); i++)
		{
			//Don't draw disabled entities
			if (!list[i]->gameObject()->GetEnabled())
				continue;

			//Prepare the material's object specific variables
			mat->PrepareMaterialObject(list[i]->gameObject());

			// Finally do the actual drawing
			//  - Do this ONCE PER OBJECT you intend to draw
			//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
			//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
			//     vertices in the currently set VERTEX BUFFER
			context->DrawIndexed(
				mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // Offset to add to each index when looking up vertices
		}
	}
	//context->OMSetDepthStencilState(0, 0);
}

// Draw transparent objects
//TODO: Sort objects by distance from camera
void Renderer::DrawTransparentObjects(ID3D11DeviceContext * context, Camera * camera)
{
	if (transparentObjList.size() < 1)
		return;

	//Set render states
	context->OMSetBlendState(transparentBlendState, 0, 0xFFFFFFFF);
	context->OMSetDepthStencilState(transparentDepthState, 0);

	//Loop through each entity in the transparent list
	for (size_t i = 0; i < transparentObjList.size(); i++)
	{
		//Don't draw disabled entities
		if (!transparentObjList[i]->gameObject()->GetEnabled())
			continue;

		Material* mat = transparentObjList[0]->GetMaterial();
		Mesh* mesh = transparentObjList[0]->GetMesh();

		// Turn shaders on
		mat->GetVertexShader()->SetShader();
		mat->GetPixelShader()->SetShader();

		//Prepare the material's combo specific variables
		mat->PrepareMaterialCombo(transparentObjList[0]->gameObject(), camera);

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Prepare the material's object specific variables
		mat->PrepareMaterialObject(transparentObjList[i]->gameObject());

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	// Reset states
	context->OMSetDepthStencilState(0, 0);
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
}

template <typename T, typename F>
static inline void DrawShapeList(std::vector<T>* list, F drawFunc, 
	PrimitiveBatch<VertexPositionColor>* batch, float deltaTime)
{
	auto iter = list->end();
	while (iter > list->begin())
	{
		iter--;
		drawFunc(batch, *iter, Colors::LightGreen);
		
		if (iter->type == ShapeDrawType::ForDuration)
			iter->duration -= deltaTime;
		//Erase objs that need to be
		if (iter->type == ShapeDrawType::SingleFrame ||
			(iter->type == ShapeDrawType::ForDuration && iter->duration <= 0))
			iter = list->erase(iter);
	}
}
// Draw debug shapes
void Renderer::DrawDebugShapes(ID3D11DeviceContext* context, Camera* camera, float deltaTime)
{
	if (debugCubes.size() < 1 && debugSpheres.size() < 1 && debugCapsules.size() < 1
		&& debugRays.size() < 1)
		return;
	
	db_effect->SetProjection(XMLoadFloat4x4(&camera->GetRawProjectionMatrix()));
	db_effect->SetView(XMLoadFloat4x4(&camera->GetRawViewMatrix()));

	context->OMSetBlendState(db_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->RSSetState(db_states->CullNone());

	db_effect->Apply(context);

	context->IASetInputLayout(db_inputLayout.Get());

	//Do the actual drawing
	db_batch->Begin();

	DrawShapeList<ShapeXMFloat3Data>(&debugCubes, &DrawCube, db_batch.get(), deltaTime);
	DrawShapeList<ShapeFloat1Data>(&debugSpheres, &DrawSphere, db_batch.get(), deltaTime);
	DrawShapeList<ShapeXMFloat2Data>(&debugCapsules, &DrawCapsule, db_batch.get(), deltaTime);
	DrawShapeList<ShapeFloat1Data>(&debugRays, &DrawRay, db_batch.get(), deltaTime);

	db_batch->End();

	//Cleanup
	context->IASetInputLayout(0);
	context->RSSetState(0);
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::DrawSky(ID3D11DeviceContext* context, Camera* camera)
{
	//Return if we don't have a skybox
	if (!skyboxMat)
		return;

	// Set up the shaders
	skyboxMat->PrepareMaterialCombo(nullptr, camera);

	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = cubeMesh->GetVertexBuffer();
	ID3D11Buffer* indexBuffer = cubeMesh->GetIndexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set up any new render states
	context->RSSetState(skyRasterState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	// Draw
	context->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);

	// Reset states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

// Add an entity to the render list
void Renderer::AddMeshRenderer(MeshRenderer* mr)
{
	//Get identifier
	std::string identifier = mr->GetMatMeshIdentifier();

	//Check if we already have the mat/mesh combo in the map
	if (renderMap.find(identifier) != renderMap.end())
	{
		//Get render list
		std::vector<MeshRenderer*> list = renderMap[identifier];

		//Check the iterator of the entity
		if (std::find(list.begin(), list.end(), mr) != list.end())
		{
			printf("Cannot add entity %s because it is already in renderer", mr->gameObject()->GetName().c_str());
			return;
		}

		//Add to the list
		renderMap[identifier].push_back(mr);
	}
	//else make a new entry
	else
	{
		std::vector<MeshRenderer*> list;
		list.push_back(mr);
		renderMap.emplace(identifier, list);
	}
}

// Remove an entity from the render list
void Renderer::RemoveMeshRenderer(MeshRenderer* mr)
{
	//Get iterator
	std::string identifier = mr->GetMatMeshIdentifier();
	auto mapIt = renderMap.find(identifier);

	//Check if we are in the map
	if (mapIt == renderMap.end())
	{
		printf("Cannot remove entity because it is not in renderer");
		return;
	}

	//Get correct render list
	std::vector<MeshRenderer*> list = renderMap[identifier];

	//Get the iterator of the entity
	std::vector<MeshRenderer*>::iterator listIt = std::find(list.begin(), list.end(), mr);

	//Check if we are in the list
	if (listIt == list.end())
	{
		printf("Cannot remove entity because it is not in renderer");
		return;
	}

	//Swap it for the last one
	std::swap(*listIt, list.back());

	//Pop the last one
	list.pop_back();

	//Check if the list is empty
	if (list.size() == 0)
	{
		//Erase
		renderMap.erase(mr->GetMatMeshIdentifier());
	}
}


#pragma region Debug Shape Drawing
// Tell the renderer to render a cube this frame
void Renderer::AddDebugCube(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale,
	ShapeDrawType drawType, float duration)
{
	if (drawType == ShapeDrawType::None) return;
	debugCubes.push_back(ShapeXMFloat3Data(scale, position, rotation, drawType, duration));
}

// Tell the renderer to render a sphere this frame
void Renderer::AddDebugSphere(XMFLOAT3 position, XMFLOAT4 rotation, float radius,
	ShapeDrawType drawType, float duration)
{
	if (drawType == ShapeDrawType::None) return;
	debugSpheres.push_back(ShapeFloat1Data(radius, position, rotation, drawType, duration));
}

// Tell the renderer to render a capsule this frame
void Renderer::AddDebugCapsule(float radius, float height, XMFLOAT3 position, XMFLOAT4 rotation,
	ShapeDrawType drawType, float duration)
{
	if (drawType == ShapeDrawType::None) return;

	XMVECTOR posVec = XMLoadFloat3(&position);
	XMVECTOR rotVec = XMLoadFloat4(&rotation);

	//Top sphere
	XMFLOAT3 topSpherePos;
	XMStoreFloat3(&topSpherePos, XMVectorAdd(posVec,
		XMVector3Rotate(XMVectorSet(height - radius, 0, 0, 0), rotVec)));
	debugSpheres.push_back(ShapeFloat1Data(radius, topSpherePos, rotation, drawType, duration));

	//Bottom sphere
	XMFLOAT3 botSpherePos;
	XMStoreFloat3(&botSpherePos, XMVectorAdd(posVec,
		XMVector3Rotate(XMVectorSet(-(height - radius), 0, 0, 0), rotVec)));
	debugSpheres.push_back(ShapeFloat1Data(radius, botSpherePos, rotation, drawType, duration));

	debugCapsules.push_back(ShapeXMFloat2Data(XMFLOAT2(radius, height), position, rotation, drawType, duration));
}

// Tell the renderer to render a ray this frame
void Renderer::AddDebugRay(float length, XMFLOAT3 position, XMFLOAT3 direction, 
	ShapeDrawType drawType, float duration)
{
	if (drawType == ShapeDrawType::None) return;
	debugRays.push_back(ShapeFloat1Data(length, position, XMFLOAT4(direction.x, direction.y, direction.z, 0),
		drawType, duration));
}
#pragma endregion

// Set the clear color.
void Renderer::SetClearColor(const float color[4])
{
	for (int i = 0; i < 4; i++)
	{
		clearColor[i] = color[i];
	}
}

// Set the clear color.
void Renderer::SetClearColor(float r, float g, float b, float a)
{
	clearColor[0] = r;
	clearColor[1] = g;
	clearColor[2] = b;
	clearColor[3] = a;
}