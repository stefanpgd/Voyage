struct BackgroundSettings
{
    float3 bottomColor;
    float3 topColor;
    float virtualPlaneHeight;
    float bottomMaxA;
    float topMaxA;
};
ConstantBuffer<BackgroundSettings> settings : register(b0);
RWTexture2D<float4> targetTexture : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint width;
    uint height;
    targetTexture.GetDimensions(width, height);
    
    if (dispatchThreadID.x > width || dispatchThreadID.y > height)
    {
        return;
    }
    
    targetTexture[dispatchThreadID.xy] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 forward = float3(0.0, 0.0, -1.0);
    float focalLength = 1.0;
    float3 cameraCenter = float3(0.0, 0.0, 0.0);
    
    float3 screenCenter = cameraCenter + forward * focalLength;
    
    float aspectRatio = float(width) / float(height);
    float virtualPlaneHeight = settings.virtualPlaneHeight;
    float virtualPlaneWidth = virtualPlaneHeight * aspectRatio;
    
    float3 bottomLeftCorner = screenCenter - float3(virtualPlaneWidth * 0.5f, virtualPlaneHeight * 0.5f, 0.0f);
    float stepX = virtualPlaneWidth / float(width);
    float stepY = virtualPlaneHeight / float(height);
    
    float3 virtualPlaneTarget = bottomLeftCorner + float3(stepX * dispatchThreadID.x, stepY * dispatchThreadID.y, 0.0f);
    float3 rayD = virtualPlaneTarget;

    float a = smoothstep(settings.bottomMaxA, settings.topMaxA, -rayD.y);
    float3 outputColor = lerp(settings.bottomColor, settings.topColor, a);
    
    targetTexture[dispatchThreadID.xy] = float4(outputColor, 1.0);
}