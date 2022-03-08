#pragma once
#include "../macros.hpp"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "VertexStruct.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
namespace Haku
{
namespace Renderer
{
class D3D12VertexBuffer
{
public:
	D3D12VertexBuffer(
		Haku::Renderer::D3D12RenderDevice* Device,
		Haku::Renderer::D3D12CommandQueue* CommandQueue,
		VertexData*						   ptr,
		size_t							   size);
	~D3D12VertexBuffer();
	void SetBuffer(Haku::Renderer::D3D12CommandQueue* CommandQueue) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			   m_VertexBufferView;
};

class D3D12ConstBuffer
{
public:
	D3D12ConstBuffer(Haku::Renderer::D3D12RenderDevice* Device, ID3D12DescriptorHeap* UI_Desciptor);
	~D3D12ConstBuffer();
	void Update(DirectX::XMMATRIX& ref) noexcept;
	void Update(float* rotate, float* translate, float width, float height) noexcept;
	void SetBuffer(D3D12CommandQueue* CommandQueue, ID3D12DescriptorHeap* Heap, UINT slot);

private:
	uint8_t*							   m_ptr;
	ConstData							   Data{};
	Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstBuffer;
};

} // namespace Renderer
} // namespace Haku
