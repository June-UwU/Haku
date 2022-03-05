#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include <dxgi.h>
#include <d3dcompiler.h>
#include "D3D12PipelineState.hpp"
#include "../Core/Exceptions.hpp"
namespace Haku
{
namespace Renderer
{
D3D12PipelineState::D3D12PipelineState(
	D3D12RenderDevice*	 Device,
	ID3D12RootSignature* RootSignature,
	std::wstring		 VertexShader,
	std::wstring		 PixelShader)
{
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

	Microsoft::WRL::ComPtr<ID3DBlob> error;
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	HAKU_SOK_ASSERT_CHAR_PTR(D3DCompileFromFile(
		VertexShader.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error),error);
	HAKU_SOK_ASSERT_CHAR_PTR(D3DCompileFromFile(
		PixelShader.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error),error);
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCORRD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc {};
	psoDesc.NumRenderTargets				   = 1;
	psoDesc.SampleDesc.Count				   = 1;
	psoDesc.SampleDesc.Quality				   = 0;
	psoDesc.DepthStencilState.DepthEnable	   = TRUE;
	psoDesc.SampleMask						   = UINT_MAX;
	psoDesc.pRootSignature					   = RootSignature;
	psoDesc.DSVFormat						   = DXGI_FORMAT_D32_FLOAT;
	psoDesc.RTVFormats[0]					   = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DepthStencilState.DepthWriteMask   = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc		   = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.BlendState						   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState					   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.PrimitiveTopologyType			   = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.PS								   = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.VS								   = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.InputLayout						   = { inputElementDescs, _countof(inputElementDescs) };
	HAKU_SOK_ASSERT(Device->get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
}

void D3D12PipelineState::ShutDown() noexcept
{
	m_PipelineState.Reset();
}

void D3D12PipelineState::SetPipelineState(D3D12CommandQueue& Command)
{
	Command.Reset(m_PipelineState.Get());
}

} // namespace Renderer
} // namespace Haku
