#include "D3D12Buffer.hpp"
#include "../../Core/Exceptions.hpp"

namespace Haku
{
namespace Renderer
{
D3D12VertexBuffer::D3D12VertexBuffer(
	D3D12RenderDevice& Device,
	D3D12CommandQueue& CommandQueue,
	VertexData*		   ptr,
	size_t			   size)
	: VertexBuffer()
{
	auto								   uploadheapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto								   defheapprop	  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto								   resdesc		  = CD3DX12_RESOURCE_DESC::Buffer(size);
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadbuffer;
	HAKU_SOK_ASSERT(Device.get()->CreateCommittedResource(
		&uploadheapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadbuffer)));

	HAKU_SOK_ASSERT(Device.get()->CreateCommittedResource(
		&defheapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)));
	UINT8*		  pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	HAKU_SOK_ASSERT(uploadbuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, ptr, size);

	D3D12_SUBRESOURCE_DATA bufferData{};
	bufferData.pData	  = pVertexDataBegin;
	bufferData.RowPitch	  = size;
	bufferData.SlicePitch = size;
	UpdateSubresources(CommandQueue.GetCommandList(), m_VertexBuffer.Get(), uploadbuffer.Get(), 0, 0, 1, &bufferData);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	CommandQueue.GetCommandList()->ResourceBarrier(1, &barrier);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes  = sizeof(VertexData);
	m_VertexBufferView.SizeInBytes	  = size;
	uploadbuffer->Unmap(0, nullptr);

	CommandQueue.Close();
	CommandQueue.Execute();
	CommandQueue.Synchronize();
}
void D3D12VertexBuffer::SetBuffer(Haku::Renderer::D3D12CommandQueue& CommandQueue) noexcept
{
	CommandQueue.GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBufferView);
}
} // namespace Renderer
} // namespace Haku
