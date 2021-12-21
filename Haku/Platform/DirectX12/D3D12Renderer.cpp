#include "D3D12Renderer.hpp"
#include "d3dcompiler.h"
#include "../../Core/Exceptions.hpp"
#include "../../Core/Application.hpp"

#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")


// TEMP
#include "..\..\imgui\backends\imgui_impl_dx12.h"

namespace Haku
{
namespace Renderer
{
DX12Renderer::DX12Renderer()
	: Renderer(height, width)
	, m_Device()
	, m_Command(m_Device)
	, m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
	, m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
{
}
DX12Renderer::DX12Renderer(uint32_t height, uint32_t width)
	: Renderer(height, width)
	, m_Device()
	, m_Command(m_Device)
	, m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
	, m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
{
}
void DX12Renderer::Render()
{
	Commands();
	m_Command.Execute();
	m_Device.Render();
	m_Command.Synchronize();
	m_Device.FrameIndexReset();
	// Synchronize();
}
void DX12Renderer::Init()
{
	auto window = Haku::Application::Get()->GetWindow();
	height = window->GetHeight();
	width  = window->GetWidth();
	m_Device.init(window, m_Command.GetCommandQueue());

	{
		D3D12_DESCRIPTOR_HEAP_DESC srvdesc{};
		srvdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvdesc.NumDescriptors = 1;
		HAKU_SOK_ASSERT(m_Device.get()->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&UI_Desciptor)))
	}

	// creating a command issuer
	// TO BE REMOVED
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
		HAKU_SOK_ASSERT(m_Device.get()->CreateRootSignature(
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
		HAKU_SOK_ASSERT(m_Device.get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	m_Command.CommandListCreate(m_Device, m_PipelineState.Get());
	m_Command.Close();
	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		VertexData triangleVertices[] = { { { 0.0f, 0.25f, 0.0f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
										  { { 0.25f, -0.25f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
										  { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not
		// recommended. Every time the GPU needs it, the upload heap will be marshalled
		// over. Please read up on Default Heap usage. An upload heap is used here for
		// code simplicity and because there are very few verts to actually transfer.
		auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resdesc  = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		HAKU_SOK_ASSERT(m_Device.get()->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)));

		UINT8*		  pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0); 
		HAKU_SOK_ASSERT(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_VertexBuffer->Unmap(0, nullptr);

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes  = sizeof(VertexData);
		m_VertexBufferView.SizeInBytes	  = vertexBufferSize;
	}
	m_Command.Synchronize();
}

void DX12Renderer::Commands()
{
	m_Command.Reset(m_PipelineState.Get());

	m_Command.GetCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
	ID3D12DescriptorHeap* ppHeaps[] = { UI_Desciptor.Get() };
	m_Command.GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_Command.GetCommandList()->RSSetViewports(1, &m_Viewport);
	m_Command.GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

	m_Device.RenderTarget(m_Command.GetCommandList());

	m_Command.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Command.GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_Command.GetCommandList()->DrawInstanced(3, 1, 0, 0);
	
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_Command.GetCommandList());

	m_Device.BackBuffer(m_Command.GetCommandList());
	m_Command.Close();
}
} // namespace Renderer
} // namespace Haku
