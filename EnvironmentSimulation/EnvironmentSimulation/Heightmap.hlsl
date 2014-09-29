cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer terrain : register(b1)
{
	float maxHeight;
	float xOff;
	float zOff;
};

struct VertexShaderInput
{
	float3 position		: POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
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
Texture2D _bottom : register(t3);
Texture2D _middle : register(t4);
Texture2D _top : register(t5);
SamplerState displacementSampler : register(s0);

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;
	
	matrix worldViewProj = mul(mul(world, view), projection);

	input.position.y = _heightmap.SampleLevel(displacementSampler, float2(input.uv.x, input.uv.y), 0).x * maxHeight;
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	
	float4 c1 = _bottom.SampleLevel(displacementSampler, input.uv.xy * 50, 0);
	float4 c2 = _middle.SampleLevel(displacementSampler, input.uv.xy * 50, 0);
	float4 c3 = _top.SampleLevel(displacementSampler, input.uv.xy * 50, 0);

	float pc1 = saturate(1 - (input.position.y - 26.0) / 2.0);
	float pc3 = saturate((input.position.y - 32.5) / 4.5);

	float pc2;
	if (input.position.y >= 32.5)
	{
		pc2 = 1 - pc3;
		input.color = ((c2 * pc2) + (c3 * pc3)) / 2.0;
	}
	else if (input.position.y <= 28.0)
	{
		pc2 = 1 - pc1;
		input.color = ((c1 * pc1) + (c2 * pc2)) / 2.0;
	}
	else
	{
		input.color = c2;
	}
	
	output.uv = input.uv;

	float3 norm = _normalmap.SampleLevel(displacementSampler, input.uv.xy * 50, 0);

	float3 normal = mul(norm, (float3x3)transpose(world));
	float3 diffuseLightDirection = float3(0.5, 0.7, 0.2);
	float intensity = dot(normal, diffuseLightDirection);
	output.color = float4(saturate(float3(0.9, 0.9, 0.9) * intensity * float3(input.color.rgb)), 1.0);

	return output;
}