#include "Material.h"
#include <WICTextureLoader.h>
#include "Game.h"

Material::Material(wchar_t* filepath, ID3D11SamplerState* sampler, ID3D11Device* dev) :
sampler(sampler)
{
	CreateWICTextureFromFile(
		dev,
		filepath,
		0,
		&srv);
	m_Shader = new Shader();
}

Material::~Material()
{
	ReleaseMacro(srv);
	ReleaseMacro(sampler);
}

ID3D11ShaderResourceView* Material::GetSRV()
{
	return srv;
}

ID3D11SamplerState* Material::GetSampler()
{
	return sampler;
}

void Material::LoadShader(Shader* shader)
{
	m_Shader = shader;
}

void Material::LoadShader(wchar_t* filepath, ShaderType type, ID3D11Device* dev)
{
	m_Shader->LoadShader(filepath, type, dev);
}

void Material::SetShader(ID3D11DeviceContext* devCon)
{
	m_Shader->SetShader(Vert, devCon);
	m_Shader->SetShader(Pixel, devCon);
	m_Shader->SetShader(Geometry, devCon);
	m_Shader->SetShader(Compute, devCon);
	m_Shader->SetShader(Domain, devCon);
}