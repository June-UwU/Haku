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


//TEST VALUES
const uint32_t TextureHeight	= 256u;
const uint32_t TextureWidth		= 256u;
const uint32_t TexturePixelSize = 4u;

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
	std::shared_ptr<D3D12DescriptorTable> textable = std::make_shared<D3D12DescriptorTable>();
	textable->AddSRV<1, 0>();
	std::shared_ptr<D3D12RootSignatureDesc> desc = std::make_shared<D3D12RootSignatureDesc>();
	desc->AddSRV<0u, 0u>();						 // 0
	desc->AddCBV<0, 0>();						 // 1
	desc->AddCBV<1, 0>();						 // 2
	desc->AddDescriptorTable(*(textable.get())); // 3

	desc->AddSampler<0, 0>(HAKU_TEXTURE_FILTER::POINT);

	desc->AllowInputLayout();
	desc->DenyDomainShader();
	desc->DenyHullShader();
	m_Signature = std::make_unique<D3D12RootSignature>(*desc, *m_Device);

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
	TexDesc.Width			   = TextureWidth;
	TexDesc.Height			   = TextureHeight;
	TexDesc.Flags			   = D3D12_RESOURCE_FLAG_NONE;
	TexDesc.Format			   = DXGI_FORMAT_B8G8R8A8_UNORM;
	TexDesc.Dimension		   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	auto heap_prop			   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HAKU_SOK_ASSERT(m_Device->get()->CreateCommittedResource(
		&heap_prop, D3D12_HEAP_FLAG_NONE, &TexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Texture)))
	uploadBuffersize = GetRequiredIntermediateSize(m_Texture, 0, 1);

	auto upload_heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto uploadresdesc	 = CD3DX12_RESOURCE_DESC::Buffer(uploadBuffersize);
	HAKU_SOK_ASSERT(m_Device->get()->CreateCommittedResource(
		&upload_heapprop,
		D3D12_HEAP_FLAG_NONE,
		&uploadresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture_uploadheap)))

	std::vector<UINT8>	   texture	   = GenerateTextureData();
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData				   = texture.data();
	textureData.RowPitch			   = TextureWidth * TexturePixelSize;
	textureData.SlicePitch			   = textureData.RowPitch * TextureHeight;
	UpdateSubresources(m_Command->GetCommandList(), m_Texture, texture_uploadheap, 0, 0, 1, &textureData);
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_Texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	m_Command->GetCommandList()->ResourceBarrier(1, &barrier);

	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Format					= TexDesc.Format;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_DescriptorHeap->GetSRVCPUHandle());

	rtvHandle.Offset(1, m_Device->get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	m_Device->get()->CreateShaderResourceView(m_Texture, &srvDesc, rtvHandle);

	m_Texture->SetName(L"actual texture");
	texture_uploadheap->SetName(L"upload heap");
	Microsoft::WRL::ComPtr<ID3D12DebugCommandList> Check;
	m_Command->GetCommandList()->QueryInterface(IID_PPV_ARGS(&Check));
	if (Check)
	{
		auto ret = Check->AssertResourceState(m_Texture, 0, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		if (ret)
		{
			HAKU_LOG_WARN("Transition was Done");
		}
	}


	m_Command->Close();
	m_Command->Execute();
	m_Command->Synchronize();
	m_Buffer   = new D3D12VertexBuffer(m_Device, m_Command, triangleVertices, vertexBufferSize);
	m_Camera   = new D3D12ConstBuffer(m_Device, m_DescriptorHeap->GetSRVDescriptorHeap());
	m_Constant = new D3D12ConstBuffer(m_Device, m_DescriptorHeap->GetSRVDescriptorHeap());
}

std::vector<UINT8> DX12Renderer::GenerateTextureData()
{
	const UINT rowPitch	   = TextureWidth * TexturePixelSize;
	const UINT cellPitch   = rowPitch >> 3;		// The width of a cell in the checkboard texture.
	const UINT cellHeight  = TextureWidth >> 3; // The height of a cell in the checkerboard texture.
	const UINT textureSize = rowPitch * TextureHeight;

	std::vector<UINT8> data(textureSize);
	UINT8*			   pData = &data[0];

	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
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

	CD3DX12_GPU_DESCRIPTOR_HANDLE rtvHandle(m_DescriptorHeap->GetSRVGPUHandle());

	rtvHandle.Offset(1,m_Device->get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	m_Command->GetCommandList()->SetGraphicsRootDescriptorTable(3, rtvHandle);
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
