//
// Abstract class for defining game entities
//

#include "GameObject.h"
#include "Game.h"

GameObject::GameObject(Mesh* mesh):
mesh(mesh)
{
	vBuffer = mesh->GetVertexBuffer();
	iBuffer = mesh->GetIndexBuffer();

	stride = sizeof(Vertex);
	offset = 0;

	worldMat = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };
	position = { 0.0, 0.0, 0.0 };
	rotation = { 0.0, 0.0, 0.0 };
	scale = { 1.0, 1.0, 1.0 };
}

GameObject::GameObject(Material* mat) :
mat(mat)
{
	srv = mat->GetSRV();
	sampler = mat->GetSampler();

	stride = sizeof(Vertex);
	offset = 0;

	XMStoreFloat4x4(&worldMat, XMMatrixIdentity());
	position = { 0.0, 0.0, 0.0 };
	rotation = { 0.0, 0.0, 0.0 };
	scale = { 1.0, 1.0, 1.0 };
}

GameObject::GameObject(Mesh* mesh, Material* mat) :
mesh(mesh),
mat(mat)
{
	vBuffer = mesh->GetVertexBuffer();
	iBuffer = mesh->GetIndexBuffer();
	srv = mat->GetSRV();
	sampler = mat->GetSampler();

	stride = sizeof(Vertex);
	offset = 0;

	worldMat = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };
	position = { 0.0, 0.0, 0.0 };
	rotation = { 0.0, 0.0, 0.0 };
	scale = { 1.0, 1.0, 1.0 };
}

GameObject::~GameObject()
{
	ReleaseMacro(vBuffer);
	ReleaseMacro(iBuffer);
	ReleaseMacro(srv);
	ReleaseMacro(sampler);
}

void GameObject::Update(float dt)
{
	worldMat = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	// Do the math!
	//XMStoreFloat4x4(&worldMat, XMMatrixMultiply(XMLoadFloat4x4(&worldMat), XMMatrixTranslationFromVector(XMLoadFloat3(&position))));
	worldMat._11 = scale.x;
	worldMat._22 = scale.y;
	worldMat._33 = scale.z;
}

void GameObject::Draw(ID3D11DeviceContext* devCon)
{
	mat->SetShader(devCon);
	if (vBuffer)
		devCon->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	if (iBuffer)
		devCon->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
	if (srv)
	{
		devCon->VSSetShaderResources(0, 1, &srv);
		devCon->PSSetShaderResources(0, 1, &srv);
	}	
	if (sampler)
	{
		devCon->VSSetSamplers(0, 1, &sampler);
		devCon->PSSetSamplers(0, 1, &sampler);
	}
	worldMat._24 = position.y;
	devCon->DrawIndexed(mesh->numIndices, 0, 0);
}

void GameObject::SetPosition(XMFLOAT3 newPosition)
{
	position = newPosition;
}

void GameObject::SetSampler(ID3D11SamplerState* _sampler)
{
	sampler = _sampler;
}

void GameObject::SetSRV(ID3D11ShaderResourceView* _srv)
{
	srv = _srv;
}

XMFLOAT4X4 const GameObject::GetWorldMatrix() { return worldMat; }