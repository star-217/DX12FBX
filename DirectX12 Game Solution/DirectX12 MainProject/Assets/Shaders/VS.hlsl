struct VSOUT
{
    float4 svpos     : SV_POSITION;
    float4 normal    : NORMAL;
    float4 color     : COLOR;
    float2 uv        : TEXTURE;
    //float4 bones     : BONE_INDEX;
    //float4 weights   : BONE_WEIGHT;
};

cbuffer cbuff:register(b0) {

    matrix transform;
	//matrix bone[255];
	float4 ambient;
	float4 diffuse;
}

VSOUT BasicVS(float4 pos : POSITION,float4 normal : NORMAL,float4 color : COLOR,float2 uv :TEXTURE
    /*float4 bones : BONE_INDEX,
    float4 weights   : BONE_WEIGHT*/)
{
	VSOUT vsout;

    vsout.svpos = mul(transform, pos);
    normal.w = 0;
    vsout.normal = mul(transform, normal);
    vsout.color = color;

    return vsout;
}
