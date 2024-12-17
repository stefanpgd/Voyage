#include "Projects/Voyage/WaterRenderStage.h"
#include "Graphics/DXObjects.h"
#include "Graphics/DXComponents.h"
#include "Graphics/DXBuffer.h"
#include "Framework/Scene.h"
#include <imgui.h>

WaterRenderStage::WaterRenderStage(Scene* activeScene) : activeScene(activeScene)
{
	InitializePipeline();
	GenerateWaterPlane();

	DXBufferProperties properties;
	properties.isConstantBuffer = false;
	properties.isStructuredBuffer = true;
	properties.isCPUAccessible = true;

	waterSettingsBuffer = new DXBuffer(properties, &waterSettings, 1, sizeof(WaterSettings));
}

void WaterRenderStage::Update(float deltaTime)
{
	ImGui::Begin("Water Settings");
	ImGui::DragFloat("Amplitude", &waterSettings.amplitude, 0.01f);
	ImGui::DragFloat("Frequency", &waterSettings.frequency, 0.01f);
	ImGui::DragFloat("Phase", &waterSettings.phase, 0.01f);
	ImGui::End();

	waterSettings.time += deltaTime;
	waterSettingsBuffer->UpdateData(&waterSettings);
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

	glm::mat4 modelMatrix = waterTransform.GetModelMatrix();
	glm::mat4 MVP = activeScene->Camera->GetViewProjectionMatrix() * modelMatrix;

	commandList->SetGraphicsRoot32BitConstants(0, 16, &MVP, 0);
	commandList->SetGraphicsRoot32BitConstants(0, 16, &modelMatrix, 16);
	commandList->SetGraphicsRootShaderResourceView(1, waterSettingsBuffer->GetGPUVirtualAddress());

	commandList->IASetVertexBuffers(0, 1, &waterTestPlane->GetVertexBufferView());
	commandList->IASetIndexBuffer(&waterTestPlane->GetIndexBufferView());
	commandList->DrawIndexedInstanced(waterTestPlane->GetIndicesCount(), 1, 0, 0, 0);
}

void WaterRenderStage::GenerateWaterPlane()
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	int quadIndex = 0;
	unsigned int waterPlaneSize = 200;
	float scale = 0.1;

	for(int x = 0; x < waterPlaneSize; x++)
	{
		for(int y = 0; y < waterPlaneSize; y++)
		{
			float xOffset = x * scale;
			float yOffset = y * -scale;

			Vertex v0, v1, v2, v3;
			v0.Position = glm::vec3(xOffset + scale, 0.0, yOffset);
			v1.Position = glm::vec3(xOffset, 0.0, yOffset - scale);
			v2.Position = glm::vec3(xOffset, 0.0, yOffset);
			v3.Position = glm::vec3(xOffset + scale, 0.0, yOffset - scale);

			v0.TextureCoord0 = glm::vec2(1.0, 0.0);
			v1.TextureCoord0 = glm::vec2(0.0, 1.0);
			v2.TextureCoord0 = glm::vec2(0.0, 0.0);
			v3.TextureCoord0 = glm::vec2(1.0, 1.0);

			// Quad vertices 
			vertices.push_back(v0);
			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);

			// Tri - 1
			indices.push_back(quadIndex);
			indices.push_back(quadIndex + 1);
			indices.push_back(quadIndex + 2);

			// Tri - 2
			indices.push_back(quadIndex);
			indices.push_back(quadIndex + 3);
			indices.push_back(quadIndex + 1);

			quadIndex += 4;
		}
	}

	waterTestPlane = new Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
}

void WaterRenderStage::InitializePipeline()
{
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	rootParameters[0].InitAsConstants(32, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX); // MVP, Model
	rootParameters[1].InitAsShaderResourceView(0, 0);

	rootSignature = new DXRootSignature(rootParameters, _countof(rootParameters), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	DXPipelineDescription pipelineDescription;
	pipelineDescription.RootSignature = rootSignature;
	pipelineDescription.VertexPath = "Source/Shaders/Water/water.vertex.hlsl";
	pipelineDescription.PixelPath = "Source/Shaders/Water/water.pixel.hlsl";

	// debugging
	pipelineDescription.UseSolidFill = false;

	waterRenderPipeline = new DXPipeline(pipelineDescription);
}