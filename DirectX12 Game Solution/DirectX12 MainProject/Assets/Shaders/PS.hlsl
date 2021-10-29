struct VSOUT {
	float4 svpos : SV_POSITION;
	float4 normal    : NORMAL;
	float4 color     : COLOR;
};

cbuffer cbuff:register(b0) {

	matrix transform;
	//matrix bone[255];
	float4 ambient;
	float4 diffuse;
}

float4 BasicPS(VSOUT vsout) : SV_TARGET
{
	float3 light = normalize(float3(1,-1,5));
	float brightness = max(dot(-light, vsout.normal), 0.0f);
	brightness = min(brightness + 0.25f, 1.0f);


	return float4(brightness, brightness, brightness, 1.0f)/**(ambient + diffuse)*/;
}