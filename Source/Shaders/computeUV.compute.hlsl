RWTexture2D<float4> targetTexture : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint width;
    uint height;
    targetTexture.GetDimensions(width, height);
    
    if(dispatchThreadID.x > width || dispatchThreadID.y > height)
    {
        return;
    }
    
    float2 uv = float2(dispatchThreadID.xy) / float2(width, height);
    targetTexture[dispatchThreadID.xy] = float4(uv, 0.0f, 1.0f);
}