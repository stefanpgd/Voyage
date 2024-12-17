#pragma once
#include "Framework/Project.h"

class Scene;
class WaterRenderStage;
class CloudRenderStage;

class VoyageProject : public Project
{
public:
	VoyageProject();

	virtual void Update(float deltaTime) override;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList4> commandList) override;

private:
	Scene* scene;

	CloudRenderStage* cloudRenderStage;
	WaterRenderStage* waterRenderStage;
};