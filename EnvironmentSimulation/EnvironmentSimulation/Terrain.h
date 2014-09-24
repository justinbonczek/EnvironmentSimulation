#ifndef TERRAIN_H
#define TERRAIN_h

#include <d3d11.h>
#include "GameObject.h"

class Terrain : public GameObject
{
public:
	Terrain(float width, float depth, UINT x, UINT z, Material* mat, wchar_t* filepath, ID3D11Device* dev);
	virtual ~Terrain();

	float GenerateHeight(float x, float z);
	void Draw(ID3D11DeviceContext* devCon);
	void LoadNormalMap(wchar_t* filepath, ID3D11Device* dev);
private:
	ID3D11ShaderResourceView* heightmap;
	ID3D11ShaderResourceView* normalmap;
	UINT numVertices;
	UINT numIndices;

	float width;
	float depth;
	UINT x;
	UINT y;
};

#endif