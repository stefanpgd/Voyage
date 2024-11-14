#include "Projects/Voyage/VoyageProject.h"
#include "Projects/Voyage/WaterRenderStage.h"
#include "Framework/Scene.h"

VoyageProject::VoyageProject()
{
	scene = new Scene(false, false);
	waterRenderStage = new WaterRenderStage(scene);
}

void VoyageProject::Update(float deltaTime)
{
	scene->Update(deltaTime);
	waterRenderStage->Update(deltaTime);
}

void VoyageProject::Render(ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	waterRenderStage->RecordStage(commandList);
}