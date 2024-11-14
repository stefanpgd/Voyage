#include "Projects/Voyage/WaterRenderStage.h"
#include "Graphics/DXObjects.h"
#include "Graphics/DXComponents.h"
#include "Framework/Scene.h"

WaterRenderStage::WaterRenderStage(Scene* activeScene) : activeScene(activeScene)
{
	waterPlane = new Model("Assets/Models/WaterPlane/plane.gltf");

	InitializePipeline();
}

void WaterRenderStage::RecordStage(ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	if(!activeScene)
	{
		LOG(Log::MessageType::Error, "Cannot render the scene if 'scene' was never initialized!");
		return;
	}

	commandList->SetGraphicsRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(waterRenderPipeline->GetAddress());

	const std::vector<Model*>& models = activeScene->GetModels();
	glm::mat4 modelMatrix = waterPlane->transform.GetModelMatrix();
	glm::mat4 MVP = activeScene->Camera->GetViewProjectionMatrix() * modelMatrix;

	commandList->SetGraphicsRoot32BitConstants(0, 16, &MVP, 0);
	commandList->SetGraphicsRoot32BitConstants(0, 16, &modelMatrix, 16);

	std::vector<Mesh*> meshes = waterPlane->GetMeshes();
	for(Mesh* mesh : meshes)
	{
		commandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
		commandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
		commandList->DrawIndexedInstanced(mesh->GetIndicesCount(), 1, 0, 0, 0);
	}
}

void WaterRenderStage::InitializePipeline()
{
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsConstants(32, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX); // MVP, Model

	rootSignature = new DXRootSignature(rootParameters, _countof(rootParameters), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	DXPipelineDescription pipelineDescription;
	pipelineDescription.RootSignature = rootSignature;
	pipelineDescription.VertexPath = "Source/Shaders/Water/water.vertex.hlsl";
	pipelineDescription.PixelPath = "Source/Shaders/Water/water.pixel.hlsl";

	waterRenderPipeline = new DXPipeline(pipelineDescription);
}