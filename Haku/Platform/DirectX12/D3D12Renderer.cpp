#include "directx/d3d12.h"
#include "directx/D3dx12.h"
#include "d3dcompiler.h"
#include "D3D12Renderer.hpp"
#include "../../Core/Exceptions.hpp"
#include "../../Core/Application.hpp"

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3DCompiler.lib")

// TEMP
#include "..\..\imgui\backends\imgui_impl_dx12.h"

namespace Haku
{
namespace Renderer
{
DX12Renderer::DX12Renderer()
	: Renderer(height, width)
	, m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
	, m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
{
	m_Device  = new D3D12RenderDevice();
	m_Command = new D3D12CommandQueue(*m_Device);
}
DX12Renderer::DX12Renderer(uint32_t height, uint32_t width)
	: Renderer(height, width)
	, m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
	, m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
{
	m_Device  = new D3D12RenderDevice();
	m_Command = new D3D12CommandQueue(*m_Device);
}
void DX12Renderer::Render()
{
	Commands();
	m_Command->Execute();
	m_Device->Render();
	m_Command->Synchronize();
	m_Device->FrameIndexReset();
	// Synchronize();
}
void DX12Renderer::Init()
{
	auto window = Haku::Application::Get()->GetWindow();
	height		= window->GetHeight();
	width		= window->GetWidth();
	m_Device->init(window, m_Command->GetCommandQueue());

	{
		D3D12_DESCRIPTOR_HEAP_DESC srvdesc{};
		srvdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvdesc.NumDescriptors = 1;
		HAKU_SOK_ASSERT(m_Device->get()->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&UI_Desciptor)))
	}

	LoadAssets();
}

void DX12Renderer::LoadAssets()
{
	// Create an empty root signature.
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		Microsoft::WRL::ComPtr<ID3DBlob> error;
		HAKU_SOK_ASSERT(
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		HAKU_SOK_ASSERT(m_Device->get()->CreateRootSignature(
			0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
	}

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
		psoDesc.pRootSignature					   = m_RootSignature.Get();
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
		HAKU_SOK_ASSERT(m_Device->get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	m_Command->CommandListCreate(*m_Device, m_PipelineState.Get());
	// Create the vertex buffer.

	// Define the geometry for a triangle.
	VertexData triangleVertices[] = { { { 0.0f, 0.25f, 0.0f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
									  { { 0.25f, -0.25f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
									  { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };

	const UINT vertexBufferSize = sizeof(triangleVertices);

	Buffer = new D3D12VertexBuffer(*m_Device, *m_Command, triangleVertices, vertexBufferSize);

}

void DX12Renderer::Commands()
{
	m_Command->Reset(m_PipelineState.Get());

	m_Command->GetCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
	ID3D12DescriptorHeap* ppHeaps[] = { UI_Desciptor.Get() };
	m_Command->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_Command->GetCommandList()->RSSetViewports(1, &m_Viewport);
	m_Command->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

	m_Device->RenderTarget(m_Command->GetCommandList());

	m_Command->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Buffer->SetBuffer(*m_Command);
	m_Command->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_Command->GetCommandList());

	m_Device->BackBuffer(m_Command->GetCommandList());
	m_Command->Close();
}
} // namespace Renderer
} // namespace Haku
