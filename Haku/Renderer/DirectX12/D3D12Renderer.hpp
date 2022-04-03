// TODO : Create and make the texture class work then convert this into a Interface that provides functions for
// everything

#pragma once
#include "macros.hpp"
#include "hakupch.hpp"
#include "Core/Exceptions.hpp"

#include "D3D12SwapChain.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12RootSignature.hpp"
#include "D3D12PipelineState.hpp"
#include "../../Platform/Windows/MainWindow.hpp"
// NOTE : this will be part of the ECS..?
#include "D3D12Resources.hpp"
#include "PerspectiveCamera.hpp"

namespace Haku
{
namespace Renderer
{
class RenderEngine
{
public:
	static void			 Render();
	static void			 ShutDown();
	static void			 EndScene(){};
	static void			 Initialize();
	static void			 BeginScene(){};
	static ID3D12Device* GetDeviceD3D() noexcept;
	static CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);

private:
	static std::unique_ptr<RenderDevice> m_Device;
	static std::unique_ptr<CommandQueue> m_CopyQueue;
	static std::unique_ptr<CommandQueue> m_CommandQueue;
	static std::unique_ptr<CommandQueue> m_ComputeQueue;
};

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

	// throw away section
private:
	std::vector<UINT8> GenerateTextureData();

private:
	float				 m_width;
	float				 m_height;
	D3D12RenderDevice*	 m_Device;
	D3D12CommandQueue*	 m_Command;
	CD3DX12_VIEWPORT	 m_Viewport;
	D3D12SwapChain*		 m_SwapChain;
	CD3DX12_RECT		 m_ScissorRect;
	D3D12DescriptorHeap* m_DescriptorHeap;

	uint32_t						uploadBuffersize;
	ID3D12Resource*					m_Texture;
	ID3D12Resource*					texture_uploadheap;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	std::unique_ptr<D3D12RootSignature> m_Signature;
	PerspectiveCamera					m_Camera_Constants;

	D3D12VertexBuffer*	m_Buffer		= nullptr;
	D3D12ConstBuffer*	m_Camera		= nullptr;
	D3D12ConstBuffer*	m_Constant		= nullptr;
	D3D12PipelineState* m_PipelineState = nullptr;
};
} // namespace Renderer
} // namespace Haku
