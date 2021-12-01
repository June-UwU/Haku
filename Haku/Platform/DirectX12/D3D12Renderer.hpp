#pragma once
#include "../../Renderer/Renderer.hpp"
#include "../Windows/MainWindow.hpp"

/*WORK TO BE DONE */
// HAKU_SOK_ASSERT ====> an exceptions

namespace Haku
{
namespace Renderer
{
class DX12Renderer : public Renderer
{
public:
	DX12Renderer() = default;
	DX12Renderer(uint32_t height, uint32_t width);
	void Cleanup() override{};
	void Render() override;
	void Update() override{};
	void Init(Haku::Windows* window);

private:
	// this fuction will be pulled out for asset management and other
	void LoadAssets();
	void Synchronize();
	void Commands();
	// this function that chooses adapters based on the condition is obselete
	// void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
	static const UINT FrameCount = 2;

	// Pipeline objects.
	// view port and the cut rect
	CD3DX12_VIEWPORT m_Viewport;
	CD3DX12_RECT	 m_ScissorRect;
	// swapchain and render devices
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device>	m_Device;
	// Render targets
	Microsoft::WRL::ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
	// Render and GPU commands
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   m_CommandQueue;
	// currently unknown functions
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		  m_RootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	  m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		  m_PipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	UINT											  m_RtvDescriptorSize;

	// App resources.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			   m_VertexBufferView;

	// Synchronization objects.
	UINT								m_FrameIndex;
	HANDLE								m_FenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	UINT64								m_FenceValue;
};
} // namespace Renderer
} // namespace Haku
