#pragma once
#include "D3D12Buffer.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12PipelineState.hpp"
#include "../Windows/MainWindow.hpp"
#include "../../Renderer/Renderer.hpp"
namespace Haku
{
namespace Renderer
{
class DX12Renderer : public Renderer
{
public:
	DX12Renderer();
	DX12Renderer(uint32_t height, uint32_t width);
	void				  Init();
	void				  Render() override;
	void				  Update() override{};
	void				  Cleanup() override{};
	ID3D12Device*		  GetDevice() { return m_Device->get(); }
	void				  Resize(uint32_t height, uint32_t width);
	ID3D12DescriptorHeap* GetDesciptor() { return UI_Desciptor.Get(); }

private:
	// this fuction will be pulled out for asset management and other
	void Commands();
	void LoadAssets();

private:
	D3D12RenderDevice* m_Device;
	D3D12CommandQueue* m_Command;

	/*RAW DIRECTX OBJECTS*/
	CD3DX12_VIEWPORT m_Viewport;
	CD3DX12_RECT	 m_ScissorRect;

	// currently unknown functions
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

	// creating a desciptor for the directx shader resouce view (srv),unordered access view and constant buffers view
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> UI_Desciptor;

	// pipeline state that is used to set the stages and shaders
	D3D12PipelineState* m_PipelineState = nullptr;
	D3D12VertexBuffer*	m_Buffer		= nullptr;
};
} // namespace Renderer
} // namespace Haku
