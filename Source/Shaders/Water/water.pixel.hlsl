struct PixelIN
{
    float3 WorldPosition : WorldPosition;
    float3 Normal : Normal;
    float2 TexCoord0 : TexCoord0;
};

float4 main(PixelIN IN) : SV_TARGET
{
    return float4(float3(IN.WorldPosition.yyy), 1.0f);
}