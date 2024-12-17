struct PixelIN
{
    float2 TexCoord0 : TexCoord0;
    float3 Normal : Normal;
};

float4 main(PixelIN IN) : SV_TARGET
{
    float3 sunDirection = normalize(float3(0.0, 1.0, 0.0));
    float diff = dot(sunDirection, IN.Normal);
    
    return float4(IN.TexCoord0.xy, 0.0f, 1.0f);
}