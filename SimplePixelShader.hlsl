/*
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
*/
/*
struct PixelShaderInput
{
	float4 Diffuse    : COLOR0;
	float2 ScreenPos  : TEXCOORD2;
	float4 PositionPS : SV_Position;
};

float hash(float n)
{
	return frac(sin(n)*43758.5453);
}

float noise(float2 x)
{
	// The noise function returns a value in the range -1.0f -> 1.0f

	float2 p = floor(x);
	float2 f = frac(x);

	f = f*f*(3.0 - 2.0*f);
	float n = p.x + p.y*57.0;

	return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
		lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
		lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
		lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), 0.0);
}

*/
Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
	/*
	float x = 0.9 + noise(10*texCoord);
	float4 t = Texture.Sample(TextureSampler, texCoord);
	t.rgb *= x;
	return float4(t.rgb, 1.0f);
	*/
	//return color;
	float Brightness = 1.0f;
	const int NUM = 9;
	const float threshold = 0.01f; //0.05;

	const float2 c[NUM] = {
		float2(-0.0025, 0.0015),
		float2(0.0025, 0.0015),
		float2(0.0025, 0.0015),
		float2(-0.0025, 0.00),
		float2(0.0, 0.0),
		float2(0.0025, 0.0),
		float2(-0.0025, -0.0015),
		float2(0.00, -0.0015),
		float2(0.0025, -0.0015),
	};

	float3 col[NUM];
	int i;
	for (i = 0; i < NUM; i++) {
		col[i] = Texture.Sample(TextureSampler, texCoord.xy + c[i]);
	}
	float3 rgb2lum = float3(0.30, 0.59, 0.11);

	float lum[NUM];
	for (i = 0; i < NUM; i++) {
		lum[i] = dot(col[i].xyz, rgb2lum);
	}
	float x = lum[2] + lum[8] + 2 * lum[5] - lum[0] - 2 * lum[3] - lum[6];
	float y = lum[6] + 2 * lum[7] + lum[8] - lum[0] - 2 * lum[1] - lum[2];

	float edge = (x*x + y*y < threshold) ? 1.0 : 0.0;
	float4 ret;
	ret.rgb = Brightness * col[5].xyz * edge.xxx;
	ret.w = 1.0;
	return ret;

}

/*
float4 main(PixelShaderInput input) : SV_TARGET
{
	float x = 0.9 + 0.05 * (noise(0.5*input.ScreenPos) + 0.5*noise(2*input.ScreenPos) + 0.25 * noise(4 * input.ScreenPos));
	input.Diffuse.rgb *= x;
	return input.Diffuse;
}
*/
