#pragma once
#include "../../Renderer/Renderer.hpp"
#include "../Windows/MainWindow.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
/*WORK TO BE DONE */
// HAKU_SOK_ASSERT ====> an exceptions

namespace Haku
{
namespace Renderer
{
class DX12Renderer : public Renderer
{
public:
	DX12Renderer();
	DX12Renderer(uint32_t height, uint32_t width);
	void				  Render() override;
	void				  Update() override{};
	void				  Cleanup() override{};
	void				  Init(Haku::Windows* window);
	ID3D12Device*		  GetDevice() { return m_Device.get(); }
	ID3D12DescriptorHeap* GetDesciptor() { return m_SCU_RV_Desciptor.Get(); }

private:
	// this fuction will be pulled out for asset management and other
	void Commands();
	void LoadAssets();

private:
	D3D12RenderDevice m_Device;
	D3D12CommandQueue m_Command;
	// Pipeline objects.
	// view port and the cut rect
	CD3DX12_VIEWPORT m_Viewport;
	CD3DX12_RECT	 m_ScissorRect;
	// swapchain and render devices

	// currently unknown functions
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

	// creating a desciptor for the directx shader resouce view (srv),unordered access view and constant buffers view
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SCU_RV_Desciptor;

	// pipeline state that is used to set the stages and shaders
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
	// commands that the gpu must execute

	// App resources.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			   m_VertexBufferView;
};
} // namespace Renderer
} // namespace Haku
