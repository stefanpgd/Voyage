#include "Graphics/DXR/DXRayTracingPipeline.h"
#include "Graphics/DXR/DXRayTracingUtilities.h"
#include "Graphics/DXR/DXTLAS.h"

#include "Graphics/DXAccess.h"
#include "Graphics/DXUtilities.h"
#include "Graphics/Texture.h"

#include "Utilities/Logger.h"

DXRayTracingPipeline::DXRayTracingPipeline(DXRayTracingPipelineSettings settings) : settings(settings)
{
	// Generate Root Signatures //
	CreateRootSignature(rayGenRootSignature, settings.rayGenParameters, settings.rayGenParameterCount, true);
	CreateRootSignature(hitRootSignature, settings.hitParameters, settings.hitParameterCount, true);
	CreateRootSignature(missRootSignature, settings.missParameters, settings.missParameterCount, true);
	CreateRootSignature(globalDummyRootSignature, nullptr, 0, false);
	CreateRootSignature(localDummyRootSignature, nullptr, 0, true);

	// Compile shaders //
	dxc::DxcDllSupport dxcHelper;
	dxcHelper.Initialize();
	dxcHelper.CreateInstance(CLSID_DxcCompiler, &compiler);
	dxcHelper.CreateInstance(CLSID_DxcLibrary, &library);
	library->CreateIncludeHandler(&dxcIncludeHandler);

	CompileShaderLibrary(rayGenLibrary, settings.rayGenPath);
	CompileShaderLibrary(hitLibrary, settings.closestHitPath);
	CompileShaderLibrary(missLibrary, settings.missPath);

	// Create pipeline & make shader binding table //
	CreatePipeline();
}

ID3D12StateObject* DXRayTracingPipeline::GetPipelineState()
{
	return pipeline.Get();
}

ID3D12StateObjectProperties* DXRayTracingPipeline::GetPipelineProperties()
{
	return pipelineProperties.Get();
}

void DXRayTracingPipeline::CreatePipeline()
{
	std::wstring rayGenSymbol = L"RayGen";
	std::wstring missSymbol = L"Miss";
	std::wstring closestHitSymbol = L"ClosestHit";
	std::wstring hitGroupSymbol = L"HitGroup";

	int objectCount = 15;
	std::vector<D3D12_STATE_SUBOBJECT> subobjects(objectCount);
	unsigned int index = 0;

	AddLibrarySubobject(subobjects, index, rayGenLibrary, &rayGenSymbol);
	AddLibrarySubobject(subobjects, index, missLibrary, &missSymbol);
	AddLibrarySubobject(subobjects, index, hitLibrary, &closestHitSymbol);

	AddHitGroupSubobject(subobjects, index, &hitGroupSymbol, &closestHitSymbol);

	const WCHAR* shaderExports[] = { rayGenSymbol.c_str(), missSymbol.c_str(), closestHitSymbol.c_str() };
	AddShaderPayloadSubobject(subobjects, index, settings.payLoadSize, settings.attributeSize, shaderExports, 3);

	const WCHAR* rayExport[] = { rayGenSymbol.c_str() };
	const WCHAR* missExport[] = { missSymbol.c_str() };
	const WCHAR* hitGroupExport[] = { hitGroupSymbol.c_str() };
	AddRootAssociationSubobject(subobjects, index, rayGenRootSignature, rayExport, 1);
	AddRootAssociationSubobject(subobjects, index, missRootSignature, missExport, 1);
	AddRootAssociationSubobject(subobjects, index, hitRootSignature, hitGroupExport, 1);

	// The pipeline construction always requires an empty global root signature
	AddDummyRootSignatureSubobjects(subobjects, index, globalDummyRootSignature, localDummyRootSignature);

	D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
	pipelineConfig.MaxTraceRecursionDepth = settings.maxRayRecursionDepth;

	D3D12_STATE_SUBOBJECT pipelineConfigObject = {};
	pipelineConfigObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	pipelineConfigObject.pDesc = &pipelineConfig;

	subobjects[index] = pipelineConfigObject;
	index++;

	// Describe the ray tracing pipeline state object
	D3D12_STATE_OBJECT_DESC pipelineDesc = {};
	pipelineDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	pipelineDesc.NumSubobjects = index; 
	pipelineDesc.pSubobjects = subobjects.data();

	ThrowIfFailed(DXAccess::GetDevice()->CreateStateObject(&pipelineDesc, IID_PPV_ARGS(&pipeline)));
	ThrowIfFailed(pipeline->QueryInterface(IID_PPV_ARGS(&pipelineProperties)));
}

void DXRayTracingPipeline::CreateRootSignature(ComPtr<ID3D12RootSignature>& rootSignature,
	D3D12_ROOT_PARAMETER* parameterData, unsigned int parameterCount, bool isLocal)
{
	D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
	rootDesc.pParameters = parameterData;
	rootDesc.NumParameters = parameterCount;
	rootDesc.Flags = isLocal ? D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE : D3D12_ROOT_SIGNATURE_FLAG_NONE; 

	ID3DBlob* pSigBlob;
	ID3DBlob* pErrorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pSigBlob, &pErrorBlob);

	if(FAILED(hr))
	{
		if(pErrorBlob)
		{
			std::string buffer = std::string((char*)pErrorBlob->GetBufferPointer());
			LOG(Log::MessageType::Error, buffer);
		}

		assert(false && "Compilation of root signature failed, read console for errors.");
	}

	ComPtr<ID3D12Device5> device = DXAccess::GetDevice();
	ThrowIfFailed(device->CreateRootSignature(0, pSigBlob->GetBufferPointer(), 
		pSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

void DXRayTracingPipeline::CompileShaderLibrary(ComPtr<IDxcBlob>& shaderLibrary, std::wstring shaderPath)
{
	UINT32 code(0);
	IDxcBlobEncoding* pShaderText(nullptr);
	IDxcOperationResult* result;

	ThrowIfFailed(library->CreateBlobFromFile(shaderPath.c_str(), &code, &pShaderText));
	ThrowIfFailed(compiler->Compile(pShaderText, shaderPath.c_str(), L"", L"lib_6_3", nullptr, 0, nullptr, 0, dxcIncludeHandler, &result));
	ThrowIfFailed(result->GetResult(&shaderLibrary));
}