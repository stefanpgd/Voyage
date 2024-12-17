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
    
    targetTexture[dispatchThreadID.xy] = float4(0.0f, 0.0f, 0.0f, 1.0f);
 
    float3 forward = float3(0.0, 0.0, -1.0);
    float focalLength = 1.0f;
    float3 cameraCenter = float3(0.0, 0.0, 0.0);
    
    float3 screenCenter = cameraCenter + forward * focalLength;
    
    float aspectRatio = float(width) / float(height);
    float virtualPlaneHeight = 1.0f;
    float virtualPlaneWidth = virtualPlaneHeight * aspectRatio;
    
    float3 topLeftCorner = screenCenter - float3(virtualPlaneWidth * 0.5f, virtualPlaneHeight * 0.5f, 0.0f);
    float pixelX = virtualPlaneWidth / float(width);
    float pixelY = virtualPlaneHeight / float(height);
    
    float3 virtualPlaneTarget = topLeftCorner + float3(pixelX * dispatchThreadID.x, pixelY * dispatchThreadID.y, 0.0f);
    float3 rayD = normalize(virtualPlaneTarget - cameraCenter);

    float3 testColor = lerp(float3(0.0, 0.0, 0.0), float3(0.2, 0.1, 0.9), saturate(rayD.y));
    targetTexture[dispatchThreadID.xy] = float4(testColor, 1.0);
}