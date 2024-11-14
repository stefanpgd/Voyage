#pragma once
#include "Graphics/RenderStage.h"

class Scene;
class Model;
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
	void InitializePipeline();

private:
	Scene* activeScene;
	WaterSettings waterSettings;

	Model* waterPlane;
	DXPipeline* waterRenderPipeline;
};