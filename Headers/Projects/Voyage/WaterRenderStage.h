#pragma once
#include "Graphics/RenderStage.h"
#include "Graphics/Transform.h"

class Scene;
class Mesh;
class DXPipeline;
class DXBuffer;

struct WaterSettings
{
	float amplitude = 0.4f;
	float frequency = 2.0f;
	float phase = 2.0f;
	float time = 0.0f;
};

class WaterRenderStage : public RenderStage
{
public:
	WaterRenderStage(Scene* activeScene);

	void Update(float deltaTime);
	void RecordStage(ComPtr<ID3D12GraphicsCommandList4> commandList) override;

private:
	void GenerateWaterPlane();
	void InitializePipeline();

private:
	Scene* activeScene;

	WaterSettings waterSettings;
	DXBuffer* waterSettingsBuffer;

	DXPipeline* waterRenderPipeline;
	Mesh* waterTestPlane;
	Transform waterTransform;
};