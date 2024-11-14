struct Vertex
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
};

struct VertexShaderOutput
{
    float3 WorldPosition : WorldPosition;
    float3 Normal : Normal;
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

float N21(float2 uv)
{
    return frac(sin(uv.x * 28.238 + uv.y * 77.21) * 3684.31);
}

float SmoothNoise(float2 uv)
{
    float2 gID = floor(uv);
    float2 localUV = smoothstep(0, 1, frac(uv));
    
    float2 bl = gID;
    float2 br = gID + float2(1.0, 0.0);
    float bc = lerp(N21(bl), N21(br), localUV.x);

    float2 tl = gID + float2(0.0, 1.0);
    float2 tr = gID + float2(1.0, 1.0);
    float tc = lerp(N21(tl), N21(tr), localUV.x);

    return lerp(bc, tc, localUV.y);
}

VertexShaderOutput main(Vertex IN)
{
    VertexShaderOutput OUT;
    
    float3 transformedPosition = mul(transformData.Model, float4(IN.Position, 1.0f)).rgb;
    float3 waveOffset = float3(0.0, 1.0f, 0.0f) * SmoothNoise(transformedPosition.xz + waterSettings.time);
    
    OUT.WorldPosition = transformedPosition + waveOffset;
    OUT.Position = mul(transformData.MVP, float4(IN.Position + waveOffset, 1.0f));
    OUT.Normal = normalize(mul(transformData.Model, float4(IN.Normal, 0.0f)).xyz);
    OUT.TexCoord0 = IN.TexCoord0;
    
    return OUT;
}