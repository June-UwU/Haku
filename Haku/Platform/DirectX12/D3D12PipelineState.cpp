#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include <dxgi.h>
#include <d3dcompiler.h>
#include "D3D12PipelineState.hpp"
#include "../../Core/Exceptions.hpp"
namespace Haku
{
namespace Renderer
{
D3D12PipelineState::D3D12PipelineState(D3D12RenderDevice& Device, ID3D12RootSignature* RootSignature)
{
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	HAKU_SOK_ASSERT(D3DCompileFromFile(
		L"D:\\Haku\\Assets\\Shaders\\vertexshader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		compileFlags,
		0,
		&vertexShader,
		nullptr));
	HAKU_SOK_ASSERT(D3DCompileFromFile(
		L"D:\\Haku\\Assets\\Shaders\\pixelshader.hlsl",
		nullptr,
		nullptr,
		"PSMain",
		"ps_5_0",
		compileFlags,
		0,
		&pixelShader,
		nullptr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout						   = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature					   = RootSignature;
	psoDesc.VS								   = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS								   = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState					   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState						   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable	   = FALSE;
	psoDesc.DepthStencilState.StencilEnable	   = FALSE;
	psoDesc.SampleMask						   = UINT_MAX;
	psoDesc.PrimitiveTopologyType			   = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets				   = 1;
	psoDesc.RTVFormats[0]					   = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count				   = 1;
	HAKU_SOK_ASSERT(Device.get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
}

void D3D12PipelineState::SetPipelineState(D3D12CommandQueue& Command)
{
	Command.ResetCommandList(m_PipelineState.Get());
}

} // namespace Renderer
} // namespace Haku