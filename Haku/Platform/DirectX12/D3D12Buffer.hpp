#pragma once
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "../../Renderer/Buffer.hpp"
#include "../../Renderer/VertexStruct.hpp"
#include "../../Renderer/VertexStruct.hpp"

namespace Haku
{
namespace Renderer
{
class D3D12VertexBuffer : public VertexBuffer
{
public:
	D3D12VertexBuffer(
		Haku::Renderer::D3D12RenderDevice* Device,
		Haku::Renderer::D3D12CommandQueue* CommandQueue,
		VertexData*						   ptr,
		size_t							   size);
	void SetBuffer(Haku::Renderer::D3D12CommandQueue* CommandQueue) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			   m_VertexBufferView;
};

class D3D12ConstBuffer : public ConstBuffer
{
public:
	D3D12ConstBuffer(Haku::Renderer::D3D12RenderDevice* Device, ID3D12DescriptorHeap* UI_Desciptor);
	~D3D12ConstBuffer();
	void Update(float* array) noexcept;
	void SetBuffer(D3D12CommandQueue* CommandQueue, ID3D12DescriptorHeap* Heap);

private:
	uint8_t*							   m_ptr;
	ConstData							   Data{};
	Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstBuffer;
};
} // namespace Renderer
} // namespace Haku
