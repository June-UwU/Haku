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
	// TODO
	// test the descriptor_table after this is working
	std::shared_ptr<D3D12DescriptorTable > textable = std::make_shared< D3D12DescriptorTable>();
	textable->AddSRV<1, 0>();
	std::shared_ptr<D3D12RootSignatureDesc> desc = std::make_shared<D3D12RootSignatureDesc>();
	desc->AddSRV<0u, 0u>(D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
	desc->AddCBV<0, 0>();
	desc->AddCBV<1, 0>();
	desc->AddDescriptorTable(*(textable.get()));
	//desc->AddSRV<1, 0>();
	desc->AddSampler<0, 0>(HAKU_TEXTURE_FILTER::POINT);
	desc->AllowInputLayout();
	desc->DenyDomainShader();
	desc->DenyHullShader();
	m_Signature = std::make_unique<D3D12RootSignature>(*desc, *m_Device);

	m_Command->CommandListCreate(*m_Device, nullptr);

	// Define the geometry for a triangle.
	VertexData triangleVertices[] = { { { 0.0f, 0.25f, 0.0f }, { 0.5f, 0.0f } },
									  { { 0.25f, -0.25f, 0.0f }, { 1.0f, 1.0f } },
									  { { -0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f } } };

	const UINT vertexBufferSize = sizeof(triangleVertices);
	m_PipelineState				= new D3D12PipelineState(
		m_Device,
		m_Signature->Get(),
		L"D:\\Haku\\Assets\\Shaders\\vertexshader.hlsl",
		L"D:\\Haku\\Assets\\Shaders\\pixelshader.hlsl");

	// testing texture and the proper usage
	D3D12_RESOURCE_DESC TexDesc{};
	TexDesc.MipLevels		   = 1;
	TexDesc.DepthOrArraySize   = 1;
	TexDesc.SampleDesc.Count   = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Width			   = 256;
	TexDesc.Height			   = 256;
	TexDesc.Flags			   = D3D12_RESOURCE_FLAG_NONE;
	TexDesc.Format			   = DXGI_FORMAT_B8G8R8A8_UNORM;
	TexDesc.Dimension		   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	auto heap_prop			   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HAKU_SOK_ASSERT(m_Device->get()->CreateCommittedResource(
		&heap_prop, D3D12_HEAP_FLAG_NONE, &TexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Texture)))
	const uint32_t uploadBuffersize = GetRequiredIntermediateSize(m_Texture.Get(), 0, 1);

	auto upload_heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto uploadresdesc	 = CD3DX12_RESOURCE_DESC::Buffer(uploadBuffersize);
	HAKU_SOK_ASSERT(m_Device->get()->CreateCommittedResource(
		&upload_heapprop,
		D3D12_HEAP_FLAG_NONE,
		&uploadresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture_uploadheap)))

	std::vector<uint8_t>   texture	   = GenerateTextureData();
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData				   = &texture[0];
	textureData.RowPitch			   = 256 * 256;
	textureData.SlicePitch			   = textureData.RowPitch * 256;
	UpdateSubresources(m_Command->GetCommandList(), m_Texture.Get(), texture_uploadheap.Get(), 0, 0, 1, &textureData);
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_Texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_Command->GetCommandList()->ResourceBarrier(1, &barrier);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format							= TexDesc.Format;
	srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels				= 1;
	m_Device->get()->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_DescriptorHeap->GetSRVCPUHandle());

	m_Texture->SetName(L"actual texture");
	texture_uploadheap->SetName(L"upload heap");

	m_Camera   = new D3D12ConstBuffer(m_Device, m_DescriptorHeap->GetSRVDescriptorHeap());
	m_Buffer   = new D3D12VertexBuffer(m_Device, m_Command, triangleVertices, vertexBufferSize);
	m_Constant = new D3D12ConstBuffer(m_Device, m_DescriptorHeap->GetSRVDescriptorHeap());

	// m_Command->Execute();
	// m_Command->Synchronize();
}

std::vector<UINT8> DX12Renderer::GenerateTextureData()
{
	const UINT rowPitch	   = 256 * 256;
	const UINT cellPitch   = rowPitch >> 3; // The width of a cell in the checkboard texture.
	const UINT cellHeight  = 256 >> 3;		// The height of a cell in the checkerboard texture.
	const UINT textureSize = rowPitch * 256;

	std::vector<UINT8> data(textureSize);
	UINT8*			   pData = &data[0];

	for (UINT n = 0; n < textureSize; n += 256)
	{
		UINT x = n % rowPitch;
		UINT y = n / rowPitch;
		UINT i = x / cellPitch;
		UINT j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n]	 = 0x00; // R
			pData[n + 1] = 0x00; // G
			pData[n + 2] = 0x00; // B
			pData[n + 3] = 0xff; // A
		}
		else
		{
			pData[n]	 = 0xff; // R
			pData[n + 1] = 0xff; // G
			pData[n + 2] = 0xff; // B
			pData[n + 3] = 0xff; // A
		}
	}

	return data;
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
