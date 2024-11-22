#include "Projects/Voyage/VoyageProject.h"
#include "Projects/Voyage/WaterRenderStage.h"
#include "Framework/Scene.h"
#include "Graphics/Camera.h"

VoyageProject::VoyageProject()
{
	scene = new Scene(false, false);
	waterRenderStage = new WaterRenderStage(scene);

	scene->Camera->Position = glm::vec3(1., 0.55f, 2.5f);
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