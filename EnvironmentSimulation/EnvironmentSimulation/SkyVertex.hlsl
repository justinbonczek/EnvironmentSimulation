cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 posL			: POSITION;
};

struct VertexToPixel
{
	float4 posH			: SV_POSITION;
	float3 posL			: POSITION;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel o;

	matrix worldViewProj = mul(mul(world, view), projection);

	o.posH = mul(float4(input.posL, 1.0f), worldViewProj).xyww;
	o.posL = input.posL;

	return o;
}