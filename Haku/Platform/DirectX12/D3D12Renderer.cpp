#include "D3D12Renderer.hpp"
#include "d3dcompiler.h"

#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")

//TEMP
#include "..\..\imgui\backends\imgui_impl_dx12.h"

namespace Haku
{
namespace Renderer
{
DX12Renderer::DX12Renderer(uint32_t height, uint32_t width)
	: Renderer(height, width)
	, m_FrameIndex(0)
	, m_Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
	, m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
	, m_RtvDescriptorSize(0)
{
}
void DX12Renderer::Render()
{
	Commands();
	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	HAKU_SOK_ASSERT(m_SwapChain->Present(1, 0));
	Synchronize();
}
void DX12Renderer::Init(Haku::Windows* window)
{
	height = window->GetHeight();
	width  = window->GetWidth();
	unsigned int FactoryFlags{};
#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> DebugDevice;
		HAKU_SOK_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugDevice)))
		FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		DebugDevice->EnableDebugLayer();
	}
#endif
	Microsoft::WRL::ComPtr<IDXGIAdapter> GraphicsAdapter; // Graphics adapter IUnknown
	Microsoft::WRL::ComPtr<IDXGIFactory6>
		DxgiFactory; // this might fail due to lack of support,might need to extend the interface
	HAKU_SOK_ASSERT(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&DxgiFactory)))
	HAKU_SOK_ASSERT(DxgiFactory->EnumAdapterByGpuPreference(
		0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&GraphicsAdapter)))
#ifdef _DEBUG
	DXGI_ADAPTER_DESC adpter_desc{};
	HAKU_SOK_ASSERT(GraphicsAdapter->GetDesc(&adpter_desc))
	// HAKU_LOG_INFO(adpter_desc.Description);

#endif

	HAKU_SOK_ASSERT(D3D12CreateDevice(
		GraphicsAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_Device))) // the support and request needs checking
								  // Command Queue Creations
	D3D12_COMMAND_QUEUE_DESC Queue_desc{};
	Queue_desc.Type	 = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	HAKU_SOK_ASSERT(m_Device->CreateCommandQueue(&Queue_desc, IID_PPV_ARGS(&m_CommandQueue)))
	// Create Swapchain
	DXGI_SWAP_CHAIN_DESC1 Swap_desc{};
	Swap_desc.Height			 = height;
	Swap_desc.Width				 = width;
	Swap_desc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
	Swap_desc.Stereo			 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Swap_desc.BufferUsage		 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Swap_desc.BufferCount		 = 2u;
	Swap_desc.Scaling			 = DXGI_SCALING_NONE; // contrevesial movement
	Swap_desc.SwapEffect		 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Swap_desc.AlphaMode			 = DXGI_ALPHA_MODE_UNSPECIFIED; // mess around with the alpha
	Swap_desc.SampleDesc.Count	 = 1;
	Swap_desc.SampleDesc.Quality = 0;

	// Swap chain flags are something to be looked at

	// basic swapchain1  interface
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
	HAKU_SOK_ASSERT(DxgiFactory->CreateSwapChainForHwnd(
		m_CommandQueue.Get(), window->GetHandle(), &Swap_desc, nullptr, nullptr, &swap_chain1))
	// Extending the swapchain interface
	HAKU_SOK_ASSERT(swap_chain1.As(&m_SwapChain))
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	// Creating RTV heaps
	{
		D3D12_DESCRIPTOR_HEAP_DESC RTVdesc{};
		RTVdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RTVdesc.NumDescriptors = FrameCount;
		RTVdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HAKU_SOK_ASSERT(m_Device->CreateDescriptorHeap(&RTVdesc, IID_PPV_ARGS(&m_RtvHeap)))
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC srvdesc{};
		srvdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvdesc.NumDescriptors = 1;
		HAKU_SOK_ASSERT(m_Device->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&m_SCU_RV_Desciptor)))
	}
	// Frame res
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Creating a render target view(RTV)
		for (size_t i = 0; i < FrameCount; i++)
		{
			HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])))
			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, cpu_handle);
			cpu_handle.Offset(1, m_RtvDescriptorSize);
		}
	}
	// creating a command issuer
	HAKU_SOK_ASSERT(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)))
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
		HAKU_SOK_ASSERT(m_Device->CreateRootSignature(
			0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
	}

	// Create the pipeline state, which includes compiling and loading shaders.
	{
		Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
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

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Describe and create the graphics pipeline state object (PSO).
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
		HAKU_SOK_ASSERT(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	// Create the command list.
	HAKU_SOK_ASSERT(m_Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_CommandAllocator.Get(),
		m_PipelineState.Get(),
		IID_PPV_ARGS(&m_CommandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	HAKU_SOK_ASSERT(m_CommandList->Close());

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
		HAKU_SOK_ASSERT(m_Device->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8*		  pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
		HAKU_SOK_ASSERT(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes  = sizeof(VertexData);
		m_VertexBufferView.SizeInBytes	  = vertexBufferSize;
	}

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		HAKU_SOK_ASSERT(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_FenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			HAKU_SOK_ASSERT(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
	Synchronize();
}
void DX12Renderer::Synchronize()
{
	const uint64_t fence_value = m_FenceValue;
	HAKU_SOK_ASSERT(m_CommandQueue->Signal(m_Fence.Get(), fence_value))
	m_FenceValue++;
	if (m_Fence->GetCompletedValue() < fence_value)
	{
		HAKU_SOK_ASSERT(m_Fence->SetEventOnCompletion(fence_value, m_FenceEvent))
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}
void DX12Renderer::Commands()
{
	// Command list allocators can only be reset when the associated
	// command lists have finished execution on the GPU; apps should use
	// fences to determine GPU execution progress.
	HAKU_SOK_ASSERT(m_CommandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command
	// list, that command list can then be reset at any time and must be before
	// re-recording.
	HAKU_SOK_ASSERT(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));

	// Set necessary state.
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	ID3D12DescriptorHeap* ppHeaps[] = { m_SCU_RV_Desciptor.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);


	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	auto resbar = CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &resbar);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->DrawInstanced(3, 1, 0, 0);

	//Imgui impl func
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.Get());
	// Indicate that the back buffer will now be used to present.
	auto resbarpres = CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &resbarpres);

	HAKU_SOK_ASSERT(m_CommandList->Close());
}
} // namespace Renderer
} // namespace Haku
