#include "directx/d3d12.h"
#include "directx/D3dx12.h"
#include <dxgidebug.h>
#include "d3dcompiler.h"
#include "D3D12Renderer.hpp"
#include "../../Core/Exceptions.hpp"
#include "../../Core/Application.hpp"

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
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
void DX12Renderer::Resize(uint32_t height, uint32_t width)
{
	m_Device->Resize(height, width, *m_Command);
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

void DX12Renderer::Close() const
{
	m_Command->Synchronize();
	m_Command->CloseFenceHandle();
	m_Command->ShutDown();
	m_Device->ShutDown();
	m_RootSignature->Release();
	UI_Desciptor->Release();
	delete m_PipelineState;
	delete m_Buffer;
#if _DEBUG
	// Check if there are still some graphics resources alive leaking.
	// If that is the case, ReportLiveObjects will trigger a breakpoint and output details in the console
	Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
	HAKU_SOK_ASSERT(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)));
	dxgiDebug->ReportLiveObjects(
		DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
#endif
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

	m_Command->CommandListCreate(*m_Device, nullptr);
	// Create the vertex buffer.

	// Define the geometry for a triangle.
	VertexData triangleVertices[] = { { { 0.0f, 0.25f, 0.0f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
									  { { 0.25f, -0.25f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
									  { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };

	const UINT vertexBufferSize = sizeof(triangleVertices);
	m_PipelineState				= new D3D12PipelineState(
		*m_Device,
		m_RootSignature.Get(),
		L"D:\\Haku\\Assets\\Shaders\\vertexshader.hlsl",
		L"D:\\Haku\\Assets\\Shaders\\pixelshader.hlsl");
	m_Buffer = new D3D12VertexBuffer(*m_Device, *m_Command, triangleVertices, vertexBufferSize);
}

void DX12Renderer::Commands()
{
	m_PipelineState->SetPipelineState(*m_Command);
	m_Command->GetCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
	ID3D12DescriptorHeap* ppHeaps[] = { UI_Desciptor.Get() };
	m_Command->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_Command->GetCommandList()->RSSetViewports(1, &m_Viewport);
	m_Command->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

	m_Device->RenderTarget(m_Command->GetCommandList());

	m_Command->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Buffer->SetBuffer(*m_Command);
	m_Command->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_Command->GetCommandList());

	m_Device->BackBuffer(m_Command->GetCommandList());
	m_Command->Close();
}

} // namespace Renderer
} // namespace Haku
