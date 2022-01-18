#include "directx/d3d12.h"
#include "directx/D3dx12.h"
#include "d3dcompiler.h"
#include "D3D12Renderer.hpp"
#include "../../Core/UILayer.hpp"
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
	m_Constant->Update(Haku::UI::LeftMenu::RotateData());
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
		srvdesc.NumDescriptors = 6600;
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
	delete m_PipelineState;
	delete m_Constant;
	delete m_Buffer;
}

void DX12Renderer::LoadAssets()
{
	// Create a root signature consisting of a descriptor table with a single CBV.
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned
		// will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (m_Device->get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)) !=
			S_OK)
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny uneccessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
														D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
														D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
														D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
														D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		D3D12_DESCRIPTOR_RANGE1 ranges{};
		ranges.NumDescriptors					 = 1;
		ranges.BaseShaderRegister				 = 0;
		ranges.RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		ranges.RegisterSpace					 = 0;
		ranges.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_DESCRIPTOR_TABLE1 cbv_table{};
		cbv_table.NumDescriptorRanges = 1;
		cbv_table.pDescriptorRanges	  = &ranges;

		D3D12_ROOT_PARAMETER1 param1[2]{};
		param1[0].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_SRV;
		param1[1].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_CBV;
		param1[1].Descriptor.RegisterSpace	= 0;
		param1[1].Descriptor.ShaderRegister = 0;
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC sigdesc{};
		sigdesc.Version					   = featureData.HighestVersion;
		sigdesc.Desc_1_1.NumParameters	   = 2;
		sigdesc.Desc_1_1.Flags			   = rootSignatureFlags;
		sigdesc.Desc_1_1.NumStaticSamplers = 0;
		sigdesc.Desc_1_1.pParameters	   = param1;
		sigdesc.Desc_1_1.pStaticSamplers   = 0;

		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		Microsoft::WRL::ComPtr<ID3DBlob> error;
		HAKU_SOK_ASSERT(
			D3DX12SerializeVersionedRootSignature(&sigdesc, featureData.HighestVersion, &signature, &error));
		HAKU_SOK_ASSERT(m_Device->get()->CreateRootSignature(
			0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
		m_RootSignature->SetName(L"Root descriptor");
	}
	m_Command->CommandListCreate(*m_Device, nullptr);
	// Create the vertex buffer.

	// Define the geometry for a triangle.
	VertexData triangleVertices[] = { { { 0.0f, 0.25f, 0.0f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
									  { { 0.25f, -0.25f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
									  { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };

	const UINT vertexBufferSize = sizeof(triangleVertices);
	m_PipelineState				= new D3D12PipelineState(
		m_Device,
		m_RootSignature.Get(),
		L"D:\\Haku\\Assets\\Shaders\\vertexshader.hlsl",
		L"D:\\Haku\\Assets\\Shaders\\pixelshader.hlsl");

	m_Constant = new D3D12ConstBuffer(m_Device, UI_Desciptor.Get());
	m_Buffer   = new D3D12VertexBuffer(m_Device, m_Command, triangleVertices, vertexBufferSize);
}

void DX12Renderer::Commands()
{
	m_PipelineState->SetPipelineState(m_Command);
	m_Command->GetCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
	ID3D12DescriptorHeap* ppHeaps[] = { UI_Desciptor.Get() };

	m_Command->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	m_Command->GetCommandList()->RSSetViewports(1, &m_Viewport);
	m_Command->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

	m_Device->RenderTarget(m_Command->GetCommandList());
	m_Command->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Constant->SetBuffer(m_Command, UI_Desciptor.Get());
	m_Buffer->SetBuffer(m_Command);
	m_Command->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_Command->GetCommandList());

	m_Device->BackBuffer(m_Command->GetCommandList());
	m_Command->Close();
}

} // namespace Renderer
} // namespace Haku
