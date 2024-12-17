#include "Projects/Voyage/CloudRenderStage.h"
#include "Graphics/DXComponents.h"
#include <imgui.h>

CloudRenderStage::CloudRenderStage()
{
	int width = window->GetWindowWidth();
	int height = window->GetWindowHeight();
	unsigned int* textureBuffer = new unsigned int[width * height];

	renderBuffer = new Texture(textureBuffer, width, height);
	delete textureBuffer;

	InitializePipeline();

	// Some default settings
	backgroundSettings.topColor = glm::vec3(0.13, 0.37, 0.55);
	backgroundSettings.bottomColor = glm::vec3(0.11, 0.13, 0.14);
	backgroundSettings.topMaxA = 0.62;
	backgroundSettings.bottomMaxA = -0.28;
	backgroundSettings.virtualPlaneHeight = 4.0;
}

void CloudRenderStage::Update(float deltaTime)
{
	ImGui::Begin("Background");

	ImGui::ColorEdit3("Top Color", &backgroundSettings.topColor[0]);
	ImGui::ColorEdit3("Bottom Color", &backgroundSettings.bottomColor[0]);

	ImGui::DragFloat("Top Max", &backgroundSettings.topMaxA, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("Bottom Max", &backgroundSettings.bottomMaxA, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("Virtual Plane Height", &backgroundSettings.virtualPlaneHeight);

	ImGui::End();
}

void CloudRenderStage::RecordStage(ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	unsigned int dispatchX = window->GetWindowWidth() / 8;
	unsigned int dispatchY = window->GetWindowHeight() / 8;

	commandList->SetComputeRootSignature(colorBackgroundRoot->GetAddress());
	commandList->SetPipelineState(colorBackgroundPipeline->GetAddress());

	commandList->SetComputeRootDescriptorTable(0, renderBuffer->GetUAV());
	commandList->SetComputeRoot32BitConstants(1, sizeof(ColorBackgroundSettings), &backgroundSettings, 0);
	commandList->Dispatch(dispatchX, dispatchY, 1);

	ComPtr<ID3D12Resource> screenBuffer = DXAccess::GetWindow()->GetCurrentScreenBuffer();
	TransitionResource(renderBuffer->GetAddress(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);
	TransitionResource(screenBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->CopyResource(screenBuffer.Get(), renderBuffer->GetAddress());

	TransitionResource(renderBuffer->GetAddress(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	TransitionResource(screenBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void CloudRenderStage::InitializePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 backgroundTextureRange[1];
	backgroundTextureRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0);

	CD3DX12_ROOT_PARAMETER1 backgroundParameters[2];
	backgroundParameters[0].InitAsDescriptorTable(1, &backgroundTextureRange[0]);
	backgroundParameters[1].InitAsConstants(sizeof(ColorBackgroundSettings), 0, 0);

	colorBackgroundRoot = new DXRootSignature(backgroundParameters, _countof(backgroundParameters));
	colorBackgroundPipeline = new DXComputePipeline(colorBackgroundRoot, "Source/Shaders/Clouds/backgroundColor.compute.hlsl");
}