cbuffer perFrame : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInput
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 uv		: TEXCOORD0;
	float3 normal   : NORMAL;
	float4 tangent  : TANGENT;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float2 uv		: TEXCOORD0;
	float3 normal   : NORMAL;
};

VertexOutput main(VertexInput input)
{
	VertexOutput o;

	matrix worldViewProj = mul(mul(world, view), projection);

	o.position = mul(float4(input.position, 1.0), worldViewProj);
	o.color = input.color;
	o.uv = input.uv;
	o.normal = input.normal;

	return o;
}