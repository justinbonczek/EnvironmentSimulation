#ifndef SKY_H
#define SKY_H

#include "GameObject.h"
#include "Mesh.h"

class Sky : public GameObject
{
public:
	Sky(Mesh* mesh, ID3D11Device* dev);
	virtual ~Sky();

	void Update(float dt);
	void Draw(ID3D11DeviceContext* devCon);

	void LoadCubeMap(wchar_t* filepath, ID3D11Device* dev);
	void SetMaterial(wchar_t* vertfilepath, wchar_t* pixelfilepath, ID3D11SamplerState* sampler, ID3D11Device* dev);
private:
	ID3D11ShaderResourceView* skymap;
	ID3D11RasterizerState* rState;
};

#endif