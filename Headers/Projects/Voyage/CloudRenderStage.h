#pragma once
#include "Graphics/RenderStage.h"
#include "Graphics/Transform.h"

class DXPipeline;
class DXBuffer;
class Texture;

struct ColorBackgroundSettings
{
	glm::vec3 bottomColor;
	BYTE stub; // alignment 
	glm::vec3 topColor;
	float virtualPlaneHeight;
	float bottomMaxA;
	float topMaxA;
};

class CloudRenderStage : public RenderStage
{
public:
	CloudRenderStage();

	void Update(float deltaTime);
	void RecordStage(ComPtr<ID3D12GraphicsCommandList4> commandList) override;

private:
	void InitializePipeline();

private:
	Texture* renderBuffer;

	ColorBackgroundSettings backgroundSettings;

	DXRootSignature* colorBackgroundRoot;
	DXComputePipeline* colorBackgroundPipeline;

	DXPipeline* cloudRenderPipeline;
};