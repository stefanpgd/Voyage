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
    float amplitude;
    float frequency;
    float phase;
    float time;
};
StructuredBuffer<WaterSettings> waterSettings : register(t0);

VertexShaderOutput main(Vertex IN)
{
    VertexShaderOutput OUT;
    
    float3 worldPos = IN.Position;
    
    WaterSettings water = waterSettings[0];
    
    // Wave equation
    float amplitude = water.amplitude;
    float frequency = water.frequency;
    float phase = water.phase;
    float time = water.time;
    float input = worldPos.x + worldPos.z;
    
    float wave = amplitude * sin(input * (2.0 / frequency) + time * phase);
    worldPos += wave;
    
    OUT.Position = mul(transformData.MVP, float4(worldPos, 1.0f));
    OUT.TexCoord0 = IN.TexCoord0;
    
    return OUT;
}