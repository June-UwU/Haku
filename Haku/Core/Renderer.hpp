#pragma once
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "wil/wrl.h"
#include "VertexStruct.hpp"
// Note that while Microsoft::WRL::ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().

class D3D12HelloTriangle
{
public:
	D3D12HelloTriangle(UINT width, UINT height, std::wstring name);

	virtual void OnInit(HWND handle);
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

private:
	void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
	static const UINT FrameCount = 2;

	// Pipeline objects.
	CD3DX12_VIEWPORT								  m_viewport;
	CD3DX12_RECT									  m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3>			  m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device>			  m_device;
	Microsoft::WRL::ComPtr<ID3D12Resource>			  m_renderTargets[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	  m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>		  m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		  m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	  m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		  m_pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT											  m_rtvDescriptorSize;

	// App resources.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			   m_vertexBufferView;

	// Synchronization objects.
	UINT								m_frameIndex;
	HANDLE								m_fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64								m_fenceValue;

	void LoadPipeline(HWND handle);
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();

	uint64_t	 width;
	uint64_t	 height;
	std::wstring name;
};
