#pragma once
#include "D3D12SwapChain.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12PipelineState.hpp"
#include "../Platform/Windows/MainWindow.hpp"
// this will be part of the ECS
#include "D3D12Buffer.hpp"
namespace Haku
{
namespace Renderer
{
class DX12Renderer
{
public:
	DX12Renderer(uint32_t height, uint32_t width);
	~DX12Renderer() noexcept;
	void				  Init();
	void				  Close() const;
	void				  Render();
	void				  Update(){}; // might be of use later for mid fame update etc.
	void				  Cleanup(){};
	ID3D12Device*		  GetDevice() { return m_Device->get(); }
	void				  Resize(uint32_t height, uint32_t width);
	void				  SetDimensions(uint32_t height, uint32_t width) noexcept;
	ID3D12DescriptorHeap* GetSRVDesciptor() { return m_DescriptorHeap->GetSRVDescriptorHeap(); }

private:
	// this function will be pulled out for asset management and other
	void Commands();
	void LoadAssets();

private:
	uint32_t			 m_width;
	uint32_t			 m_height;
	D3D12RenderDevice*	 m_Device;
	D3D12CommandQueue*	 m_Command;
	CD3DX12_VIEWPORT	 m_Viewport;
	D3D12SwapChain*		 m_SwapChain;
	CD3DX12_RECT		 m_ScissorRect;
	D3D12DescriptorHeap* m_DescriptorHeap;
	std::unique_ptr<D3D12RootSignature> m_Signature;
	

	D3D12VertexBuffer*	m_Buffer		= nullptr;
	D3D12PipelineState* m_PipelineState = nullptr;
	D3D12ConstBuffer*	m_Constant		= nullptr;

	
};
} // namespace Renderer
} // namespace Haku
