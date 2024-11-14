#pragma once
#include "Graphics/RenderStage.h"

class Scene;
class Model;
class DXPipeline;

class WaterRenderStage : public RenderStage
{
public:
	WaterRenderStage(Scene* activeScene);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList4> commandList) override;

private:
	void InitializePipeline();

private:
	Scene* activeScene;

	Model* waterPlane;
	DXPipeline* waterRenderPipeline;
};