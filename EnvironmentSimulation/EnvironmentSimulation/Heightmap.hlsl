cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position		: POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
};

Texture2D _texture : register(t0);
Texture2D _heightmap : register(t1);
Texture2D _normalmap : register(t2);
SamplerState displacementSampler : register(s0);

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix worldViewProj = mul(mul(world, view), projection);

	//input.position.y = sin(input.position.x) * 2.0f * cos(input.position.z) * 2.0f;
	input.position.y = _heightmap.SampleLevel(displacementSampler, input.uv.xy, 0).x * 100.0;
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	output.color = input.color;
	output.uv = input.uv;

	///
	// CALCULATE ADJUSTED NORMAL HERE
	///
	float3 norm = _normalmap.SampleLevel(displacementSampler, input.uv.xy, 0);

	float3 normal = mul(norm, (float3x3)transpose(world));
	float3 diffuseLightDirection = float3(0.0, 1.0, 0.0);
	float intensity = dot(normal, diffuseLightDirection);
	output.color = float4(saturate(float3(0.9, 0.9, 0.9) * intensity * float3(input.color.rgb)), 1.0);

	return output;
}