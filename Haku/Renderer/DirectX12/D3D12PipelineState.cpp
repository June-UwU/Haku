#include <dxgi.h>
#include <filesystem>
#include <d3dcompiler.h>
#include "Exceptions.hpp"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "D3D12Renderer.hpp"
#include "VertexProperties.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12PipelineState.hpp"
namespace Haku
{
namespace Renderer
{
void InitializePSOWithUISRV()
{
	char filepath[256];
	GetModuleFileNameA(NULL, filepath, 256);
	std::filesystem::path p(filepath);
	p.remove_filename();
	auto vertex_string = p.wstring();
	auto pixel_string  = p.wstring();
	vertex_string.append(L"..\\..\\Assets\\Shaders\\UIvertexshader.hlsl");
	pixel_string.append(L"..\\..\\Assets\\Shaders\\UIpixelshader.hlsl");
	ID3D10Blob* UIVertexShader_Blob = CompileShaderFile(vertex_string.c_str(), "vs_5_0");
	ID3D10Blob* UIPixelShader_Blob	= CompileShaderFile(pixel_string.c_str(), "ps_5_0");

	RootSigLayout layout;
	layout.AddRootSRV(0, 0);
	layout.AddDescriptorTableSRV(1, 1, 2);
	RootSignature* rootsig = new RootSignature(layout);

	CreatePSO(rootsig, UIVertexShader_Blob, UIPixelShader_Blob);
}
ID3DBlob* CompileShaderFile(const wchar_t* filepath, const char* target, UINT compileflags)
{
#ifdef _DEBUG
	compileflags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* ret_blob;
	ID3DBlob* err_blob;
	auto Hr = D3DCompileFromFile(filepath, nullptr, nullptr, "main", target, compileflags, 0, &ret_blob, &err_blob);
	if (err_blob)
	{
		OutputDebugStringA(reinterpret_cast<char*>(err_blob->GetBufferPointer()));
		err_blob->Release();
	}
	HAKU_SOK_ASSERT(Hr)
	return ret_blob;
}

std::pair<uint64_t, PipeLineState*> CreatePSO_ReturnKey(
	RootSignature* signature,
	ID3DBlob*	   VS,
	ID3DBlob*	   PS,
	ID3DBlob*	   DS,
	ID3DBlob*	   HS,
	ID3DBlob*	   GS)
{
	PipeLineState* ret_ptr = new PipeLineState(signature, VS, PS, DS, HS, DS);
	S_PSOMap.emplace(S_PSOID, ret_ptr);
	std::pair<uint64_t, PipeLineState*> ret_val(S_PSOID, ret_ptr);
	S_PSOID++;
	return ret_val;
}

PipeLineState* CreatePSO(RootSignature* signature, ID3DBlob* VS, ID3DBlob* PS, ID3DBlob* DS, ID3DBlob* HS, ID3DBlob* GS)
{
	PipeLineState* ret_val = new PipeLineState(signature, VS, PS, DS, HS, DS);
	S_PSOMap.emplace(S_PSOID, ret_val);
	S_PSOID++;
	return ret_val;
}
void ShutDownPSO() noexcept
{
	// Release all PSOs
	for (auto i : S_PSOMap)
	{
		i.second->Release();
	}
}
PipeLineState* GetPSO(uint64_t Key) noexcept
{
	auto ret_val = S_PSOMap.find(Key);
	return ret_val->second;
}
PipeLineState::PipeLineState(
	RootSignature* signature,
	ID3DBlob*	   VS,
	ID3DBlob*	   PS,
	ID3DBlob*	   DS,
	ID3DBlob*	   HS,
	ID3DBlob*	   GS)
	: m_VertexShader(VS)
	, m_PixelShader(PS)
	, m_DomainShader(DS)
	, m_HullShader(HS)
	, m_GeometryShader(GS)
	, m_RootSignature(signature)
{
	auto							   Device = RenderEngine::GetDeviceD3D();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC statedesc{};
	statedesc.pRootSignature = signature->Get();
	if (VS)
	{
		statedesc.VS = CD3DX12_SHADER_BYTECODE(VS);
	}
	else
	{
		HAKU_THROW("No Valid Vertex Shader")
	}
	if (PS)
	{
		statedesc.PS = CD3DX12_SHADER_BYTECODE(PS);
	}
	if (DS)
	{
		statedesc.DS = CD3DX12_SHADER_BYTECODE(DS);
	}
	if (HS)
	{
		statedesc.HS = CD3DX12_SHADER_BYTECODE(HS);
	}
	if (GS)
	{
		statedesc.GS = CD3DX12_SHADER_BYTECODE(GS);
	}
	D3D12_PIPELINE_STATE_FLAGS flags{ D3D12_PIPELINE_STATE_FLAG_NONE };

	statedesc.BlendState					   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	statedesc.SampleMask					   = UINT_MAX;
	statedesc.RasterizerState				   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	statedesc.DepthStencilState.DepthEnable	   = TRUE;
	statedesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	statedesc.DepthStencilState.DepthFunc	   = D3D12_COMPARISON_FUNC_LESS_EQUAL; // this might need change
	statedesc.DepthStencilState.StencilEnable  = FALSE;
	statedesc.InputLayout					   = InputDesc;
	statedesc.PrimitiveTopologyType			   = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	statedesc.NumRenderTargets				   = 1;
	statedesc.RTVFormats[0]					   = DXGI_FORMAT_R8G8B8A8_UNORM;
	statedesc.DSVFormat						   = DXGI_FORMAT_D32_FLOAT;
	statedesc.SampleDesc					   = sampledesc;
	statedesc.NodeMask						   = 0;
	statedesc.Flags							   = flags;
	HAKU_SOK_ASSERT(Device->CreateGraphicsPipelineState(
		&statedesc, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_PSO)))
}
PipeLineState::~PipeLineState()
{
	Release();
}
ID3D12PipelineState* PipeLineState::Get() const noexcept
{
	return m_PSO;
}
ID3D12RootSignature* PipeLineState::GetRootSignature() noexcept
{
	return m_RootSignature->Get();
}
void PipeLineState::Release() noexcept
{
	m_PSO->Release();
	m_VertexShader->Release();
	m_RootSignature->Release();
	if (m_HullShader)
	{
		m_HullShader->Release();
	}
	if (m_PixelShader)
	{
		m_PixelShader->Release();
	}
	if (m_DomainShader)
	{
		m_DomainShader->Release();
	}
	if (m_GeometryShader)

	{
		m_GeometryShader->Release();
	}
}
} // namespace Renderer
} // namespace Haku
