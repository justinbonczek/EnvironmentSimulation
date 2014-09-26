#ifndef TERRAIN_H
#define TERRAIN_h

#include <d3d11.h>
#include "GameObject.h"

__declspec(align(16))
struct BufferData
{
	float maxHeight;
	float xOff;
	float zOff;
};

class Terrain : public GameObject
{
public:
	Terrain(float width, float depth, UINT x, UINT z, Material* mat, wchar_t* filepath, ID3D11Device* dev);
	virtual ~Terrain();

	float GenerateHeight(float x, float z);
	void Draw(ID3D11DeviceContext* devCon);
	void Update(float dt);
	void LoadNormalMap(wchar_t* filepath, ID3D11Device* dev);

	void SetBufferData(float maxHeight, ID3D11Device * dev);
	void SetBufferData(float maxHeight, float xOff, float zOff, ID3D11Device* dev);
private:
	ID3D11ShaderResourceView* heightmap;
	ID3D11ShaderResourceView* normalmap;

	ID3D11Buffer* terrainBuffer;

	BufferData terrainBufferData;

	UINT numVertices;
	UINT numIndices;

	float width;
	float depth;
	UINT x;
	UINT y;
	bool animated;
};

#endif