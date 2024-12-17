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
    float3 Normal : Normal;
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
    float frequency = (2.0 / water.frequency);
    float time = water.time * water.phase;
    
    float2 direction = float2(1.0, 0.0);
    float xz = dot(direction, worldPos.xz);
    
    float wave = amplitude * sin(xz * frequency + time);
    //wave += amplitude * sin(dot(float2(1.0, 1.0), worldPos.xz) * (2.0 / frequency) * 0.6 + phase * time * 0.4);
    //wave += amplitude * sin(dot(float2(0.5, 0.5), worldPos.xz) * (2.0 / frequency) * 5.9 + phase * time * 0.7);
    //wave += amplitude * sin(dot(float2(1.0, 0.8), worldPos.xz) * (2.0 / frequency) * 2.8 + phase * time * 1.2);
    
    float2 n = frequency * amplitude * direction * cos(xz * frequency + time);
    
    worldPos.y += wave;
    
    OUT.Position = mul(transformData.MVP, float4(worldPos, 1.0f));
    OUT.TexCoord0 = IN.TexCoord0;
    OUT.Normal = normalize(float3(n, 0.0));
    return OUT;
}