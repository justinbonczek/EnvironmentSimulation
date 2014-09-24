#include "Terrain.h"
#include <vector>
#include <WICTextureLoader.h>
#include "Game.h"

Terrain::Terrain(float width, float depth, UINT n, UINT m, Material* mat, wchar_t* filepath, ID3D11Device* dev) :
GameObject(mat)
{
	UINT vertexCount = n * m;
	UINT faceCount = (n - 1)*(m - 1) * 2;

	float halfWidth = width * 0.5f;
	float halfDepth = depth * 0.5f;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	std::vector<Vertex> vertices;
	for (int i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (int j = 0; j < n; ++j)
		{
			float x = halfWidth - j * dx;
			Vertex cVert;
			cVert.Position = XMFLOAT3(x, 1.0, z);
			cVert.Normal = XMFLOAT3(0.0, 1.0, 0.0);
			cVert.Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
			cVert.UV.x = j * du;
			cVert.UV.y = i * dv;
			cVert.Color = XMFLOAT4(0.7, 0.7, 0.7, 1.0);
			vertices.push_back(cVert);
		}
	}	

	std::vector<UINT> indices;
	UINT k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices.push_back((i + 1) * n + j + 1);
			indices.push_back(i * n + j + 1);
			indices.push_back((i + 1) * n + j);
			indices.push_back((i + 1) * n + j);
			indices.push_back(i * n + j + 1);
			indices.push_back(i*n + j);
			k += 6;
		}
	}

	
	Vertex* _verts = &vertices[0];
	UINT* _inds = &indices[0];

	mesh = new Mesh(_verts, vertexCount, _inds, faceCount * 3, dev);
	vBuffer = mesh->GetVertexBuffer();
	iBuffer = mesh->GetIndexBuffer();
	numVertices = vertexCount;
	numIndices = faceCount * 3;
	CreateWICTextureFromFile(
		dev,
		filepath,
		0,
		&heightmap);
}

Terrain::~Terrain()
{
	ReleaseMacro(heightmap);
}

float Terrain::GenerateHeight(float x, float z)
{
	return 0.3f * (z * sinf(0.1f* x) + x* cosf(0.1f* z));
}

void Terrain::Draw(ID3D11DeviceContext* devCon)
{
	devCon->VSSetShaderResources(1, 1, &heightmap);
	devCon->VSSetShaderResources(2, 1, &normalmap);
	GameObject::Draw(devCon);
}

void Terrain::LoadNormalMap(wchar_t* filepath, ID3D11Device* dev)
{
	CreateWICTextureFromFile(
		dev,
		filepath,
		0,
		&normalmap);
}