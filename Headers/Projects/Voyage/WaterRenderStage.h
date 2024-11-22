#pragma once
#include "Graphics/RenderStage.h"
#include "Graphics/Transform.h"

class Scene;
class Mesh;
class DXPipeline;

struct WaterSettings
{
	float time;
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

	DXPipeline* waterRenderPipeline;
	Mesh* waterTestPlane;
	Transform waterTransform;
};