///
// Environment Simulation made by Justin Bonczek using DirectX 11
///

#include <d3dcompiler.h>

#include "Simulation.h"
#include "Vertex.h"
#include "XColors.h"
#include "Terrain.h"
#include "Timer.h"
#include "Sky.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd)
{
	Simulation simulation(hInstance);

	if (!simulation.Initialize())
		return 0;

	return simulation.Run();
}

Simulation::Simulation(HINSTANCE hInstance) : 
Game(hInstance)
{
	windowTitle = L"Environment Simulation";
	windowWidth = 1280;
	windowHeight = 720;
}

Simulation::~Simulation()
{
	for (GameObject* obj : objects)
	{
		delete obj;
		obj = 0;
	}
	ReleaseMacro(inputLayout);
	ReleaseMacro(matrixBuffer);
	ReleaseMacro(blendState);
	ReleaseMacro(solid);
	ReleaseMacro(wireframe)
}

bool Simulation::Initialize()
{
	if (!Game::Initialize())
		return false;

	LoadAssets();
	InitializePipeline();

	m_Camera.SetPosition(0.0, 60.0, -10.0);
	XMStoreFloat4x4(&(matrixBufferData.world), XMMatrixTranspose(XMMatrixIdentity()));
	return true;
}

void Simulation::LoadAssets()
{
	ID3D11SamplerState* wrapSampler;
	D3D11_SAMPLER_DESC wsd;
	ZeroMemory(&wsd, sizeof(D3D11_SAMPLER_DESC));
	wsd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	wsd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	wsd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	wsd.Filter = D3D11_FILTER_ANISOTROPIC;
	wsd.MaxAnisotropy = 0;
	wsd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	wsd.MinLOD = 0;
	wsd.MaxLOD = 0;
	wsd.MipLODBias = 0;
	dev->CreateSamplerState(&wsd, &wrapSampler);

	Material* heightmap = new Material(L"Textures/grass.png", wrapSampler, dev);
	heightmap->LoadShader(L"Heightmap.cso", Vert, dev);
	heightmap->LoadShader(L"TexturedDiffuse.cso", Pixel, dev);

	Material* waterMat = new Material(L"Textures/water.png", wrapSampler, dev);
	waterMat->LoadShader(L"WaterVertex.cso", Vert, dev);
	waterMat->LoadShader(L"WaterPixel.cso", Pixel, dev);

	MeshData skySphere;
	MeshGenerator::CreateSphere(1000.0, 2, skySphere);
	Mesh* skySphereMesh = new Mesh(skySphere, dev);
	Sky* sky = new Sky(skySphereMesh, dev);
	sky->LoadCubeMap(L"Textures/grasscube.dds", dev);
	sky->SetMaterial(L"SkyVertex.cso", L"SkyPixel.cso", wrapSampler, dev);
	objects.push_back(sky);

	Terrain* terrain = new Terrain(500.0f, 500.0f, 500, 500, heightmap, L"Textures/heightmap.png", dev);
	terrain->LoadNormalMap(L"Textures/normalmap2.png", dev);
	terrain->SetBufferData(100.0, dev);
	objects.push_back(terrain);

	Terrain* water = new Terrain(500.0f, 500.0f, 500, 500, waterMat, L"Textures/waterHeightMap.png", dev);
	water->LoadNormalMap(L"Textures/waterNormalMap.png", dev);
	water->SetBufferData(5.0, 0.0, 0.0, dev);
	water->SetPosition(XMFLOAT3(0, 27.0, 0));
	objects.push_back(water);
}

void Simulation::InitializePipeline()
{
	D3D11_INPUT_ELEMENT_DESC vDesc[] = 
	{
		{ "POSITION", NULL, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL },
		{ "COLOR", NULL, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, NULL },
		{ "TEXCOORD", NULL, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, NULL },
		{ "NORMAL", NULL, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, NULL },
		{ "TANGENT", NULL, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, NULL }
	};

	ID3DBlob* vertexByte;
	D3DReadFileToBlob(L"Default.cso", &vertexByte);

	dev->CreateInputLayout(vDesc, ARRAYSIZE(vDesc), vertexByte->GetBufferPointer(), vertexByte->GetBufferSize(), &inputLayout);

	ReleaseMacro(vertexByte);

	D3D11_BUFFER_DESC cd;
	ZeroMemory(&cd, sizeof(D3D11_BUFFER_DESC));
	cd.ByteWidth = sizeof(matrixBufferData);
	cd.Usage = D3D11_USAGE_DEFAULT;
	cd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cd.CPUAccessFlags = 0;
	cd.MiscFlags = 0;
	cd.StructureByteStride = 0;
	dev->CreateBuffer(&cd, NULL, &matrixBuffer);

	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	dev->CreateBlendState(&bd, &blendState);

	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	dev->CreateRasterizerState(&rd, &wireframe);

	rd.FillMode = D3D11_FILL_SOLID;
	dev->CreateRasterizerState(&rd, &solid);

	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = false;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dev->CreateDepthStencilState(&dsd, &depthStencilState);
}

void Simulation::OnResize()
{
	Game::OnResize();

	m_Camera.SetLens(0.25f * 3.1415926535f, AspectRatio(), 0.1f, 200.0f);
	XMStoreFloat4x4(&(matrixBufferData.projection), XMMatrixTranspose(m_Camera.Proj()));
}

void Simulation::Update(float dt)
{
	time += dt;
	if (GetAsyncKeyState(VK_SPACE) && 0x8000 && time > 0.25)
	{
		if (current == solid)
		{
			current = wireframe;
			time = 0;
		}
		else
		{
			current = solid;
			time = 0;
		}
		if (time > 100)
			time -= 100;
	}
	MoveCamera(dt);

	for (GameObject* obj : objects)
	{
		obj->Update(dt);
	}
}

void Simulation::Draw()
{
	m_Camera.UpdateViewMatrix();

	XMStoreFloat4x4(&(matrixBufferData.view), XMMatrixTranspose(m_Camera.View()));
	
	const float clearColor[4] = { 0.0f, 0.0, 0.0f, 1.0f };

	devCon->ClearRenderTargetView(renderTargetView, clearColor);
	devCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	devCon->OMSetBlendState(blendState, blendFactors, 0xFFFFFF);

	devCon->IASetInputLayout(inputLayout);
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->VSSetConstantBuffers(0, 1, &matrixBuffer);
	devCon->OMSetDepthStencilState(depthStencilState, 0);

	for (GameObject* obj : objects)
	{
		devCon->RSSetState(current);
		XMStoreFloat4x4(&(matrixBufferData.world), XMLoadFloat4x4(&(obj->GetWorldMatrix())));
		devCon->UpdateSubresource(matrixBuffer, 0, NULL, &matrixBufferData, 0, 0);
		obj->Draw(devCon);
	}
	
	swapChain->Present(0, 0);
}

void Simulation::MoveCamera(float dt)
{
	if (GetAsyncKeyState('W') & 0x8000)
		m_Camera.Walk(10.0f*dt);
	if (GetAsyncKeyState('S') & 0x8000)
		m_Camera.Walk(-10.0f*dt);
	if (GetAsyncKeyState('A') & 0x8000)
		m_Camera.Strafe(-10.0f*dt);
	if (GetAsyncKeyState('D') & 0x8000)
		m_Camera.Strafe(10.0f*dt);
	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_Camera.Pitch(-1.0f * dt);
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_Camera.RotateY(-1.0f * dt);
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_Camera.RotateY(1.0f * dt);
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_Camera.Pitch(1.0f * dt);
}