struct Vertex
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
};

struct VertexShaderOutput
{
    float2 TexCoord0 : TexCoord0;
    float4 Position : SV_Position;
};

struct TransformData
{
    matrix MVP;
    matrix Model;
};
ConstantBuffer<TransformData> transformData : register(b0);

struct WaterSettings
{
    float time;
};
ConstantBuffer<WaterSettings> waterSettings : register(b1);

VertexShaderOutput main(Vertex IN)
{
    VertexShaderOutput OUT;
    
    OUT.Position = mul(transformData.MVP, float4(IN.Position, 1.0f));
    OUT.TexCoord0 = IN.TexCoord0;
    
    return OUT;
}