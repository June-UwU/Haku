#include "directx/d3d12.h"
#include "directx/D3dx12.h"
#include "d3dcompiler.h"
#include "D3D12Renderer.hpp"
#include "../Core/UILayer.hpp"
#include "../Core/Exceptions.hpp"
#include "../Core/Application.hpp"
#include "..\..\imgui\backends\imgui_impl_dx12.h"

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace Haku
{
namespace Renderer
{
DX12Renderer::DX12Renderer(uint32_t height, uint32_t width)
	: m_width(width)
	, m_height(height)
	, m_Viewport(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height))
	, m_ScissorRect(0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height))
{
	m_SwapChain		 = new D3D12SwapChain();
	m_Device		 = new D3D12RenderDevice();
	m_Command		 = new D3D12CommandQueue(*m_Device);
	m_DescriptorHeap = new D3D12DescriptorHeap(*m_Device);
	Haku::UI::LeftMenu::SetFOV(60.0f);
	Haku::UI::LeftMenu::SetFarZ(100.0f);
	Haku::UI::LeftMenu::SetNearZ(0.01f);
	Haku::UI::LeftMenu::SetAspectRatio(1.0f);
}
DX12Renderer::~DX12Renderer() noexcept
{
	Close();
}
void DX12Renderer::Render()
{
	auto rotate	   = Haku::UI::LeftMenu::GetRotateData();
	auto translate = Haku::UI::LeftMenu::GetTranslateData();
	auto Data	   = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(RADS_32BIT(rotate[0])) * DirectX::XMMatrixRotationY(RADS_32BIT(rotate[1])) *
		 DirectX::XMMatrixRotationZ(RADS_32BIT(rotate[2])) *
		 DirectX::XMMatrixTranslation(translate[0], translate[1], translate[2] + 0.5f));
	m_Constant->Update(Data);
	m_Camera_Constants.SetFOV(Haku::UI::LeftMenu::GetFOV());
	m_Camera_Constants.SetFarZ(Haku::UI::LeftMenu::GetFarZ());
	m_Camera_Constants.SetNearZ(Haku::UI::LeftMenu::GetNearZ());
	m_Camera_Constants.SetAspectRatio(Haku::UI::LeftMenu::GetAspectRatio());

	auto camera = m_Camera_Constants.GetCamera();
	m_Camera->Update(camera);
	Commands();
	m_Command->Execute();
	m_SwapChain->Render();
	m_Command->Synchronize();
	m_SwapChain->SetBackBufferIndex();
}
void DX12Renderer::Resize(uint32_t height, uint32_t width)
{
	m_width	 = width;
	m_height = height;
	m_SwapChain->Resize(height, width, *m_Device, *m_Command, *m_DescriptorHeap);
}
void DX12Renderer::SetDimensions(uint32_t height, uint32_t width) noexcept
{
	m_width	 = width;
	m_height = height;
}
void DX12Renderer::Init()
{
	auto window = Haku::Application::Get()->GetWindow();
	m_width		= window->GetWidth();
	m_height	= window->GetHeight();
	m_SwapChain->Init(window, *m_Device, *m_Command, *m_DescriptorHeap);
	LoadAssets();
}

void DX12Renderer::Close() const
{
	m_Command->Synchronize();
	m_Command->CloseFenceHandle();
	m_Device->ShutDown();
	m_Command->ShutDown();
	m_SwapChain->ShutDown();
	m_DescriptorHeap->ShutDown();
	delete m_PipelineState;
	delete m_Constant;
	delete m_Buffer;
	delete m_Camera;
}

void DX12Renderer::LoadAssets()
{
	std::shared_ptr<D3D12RootSignatureDesc> desc = std::make_shared<D3D12RootSignatureDesc>();
	desc->AddSRV<0u, 0u>(D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
	desc->AddCBV<0, 0>();
	desc->AddCBV<1, 0>();
	desc->AllowInputLayout();
	desc->DenyDomainShader();
	desc->DenyHullShader();
	desc->DenyPixelShader();
	m_Signature = std::make_unique<D3D12RootSignature>(*desc, *m_Device);

	m_Command->CommandListCreate(*m_Device, nullptr);

	// Define the geometry for a triangle.
	VertexData triangleVertices[] = { { { 0.0f, 0.25f, 0.0f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
									  { { 0.25f, -0.25f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
									  { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };

	const UINT vertexBufferSize = sizeof(triangleVertices);
	m_PipelineState				= new D3D12PipelineState(
		m_Device,
		m_Signature->Get(),
		L"D:\\Haku\\Assets\\Shaders\\vertexshader.hlsl",
		L"D:\\Haku\\Assets\\Shaders\\pixelshader.hlsl");

	m_Camera   = new D3D12ConstBuffer(m_Device, m_DescriptorHeap->GetSRVDescriptorHeap());
	m_Buffer   = new D3D12VertexBuffer(m_Device, m_Command, triangleVertices, vertexBufferSize);
	m_Constant = new D3D12ConstBuffer(m_Device, m_DescriptorHeap->GetSRVDescriptorHeap());
}

void DX12Renderer::Commands()
{
	m_PipelineState->SetPipelineState(*m_Command);
	m_Signature->SetSignature(*m_Command);
	m_DescriptorHeap->SetDescriptorHeaps(*m_Command);

	m_Command->GetCommandList()->RSSetViewports(1, &m_Viewport);
	m_Command->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);
	m_SwapChain->SetAndClearRenderTarget(*m_Command, *m_DescriptorHeap);
	m_Command->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Constant->SetBuffer(m_Command, m_DescriptorHeap->GetSRVDescriptorHeap(), 1);
	m_Camera->SetBuffer(m_Command, m_DescriptorHeap->GetSRVDescriptorHeap(), 2);
	m_Buffer->SetBuffer(m_Command);
	m_Command->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_Command->GetCommandList());
	m_SwapChain->TransitionPresent(*m_Command);
	m_Command->Close();
}

} // namespace Renderer
} // namespace Haku
