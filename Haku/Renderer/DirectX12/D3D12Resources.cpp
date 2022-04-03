#include "directx/d3dx12.h"
#include "D3D12Resources.hpp"
#include "../../Core/Exceptions.hpp"

namespace Haku
{
namespace Renderer
{
D3D12VertexBuffer::D3D12VertexBuffer(
	D3D12RenderDevice* Device,
	D3D12CommandQueue* CommandQueue,
	VertexData*		   ptr,
	size_t			   size)
	: D3D12Resource(Device, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT, size)
{
	m_Type = D3D12RESOURCE_TYPE::VERTEX_RESOURCE;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadbuffer;
	CommandQueue->Reset(nullptr);
	HAKU_LOG_INFO("creating vertex buffer");
	auto uploadheapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto defheapprop	= CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resdesc		= CD3DX12_RESOURCE_DESC::Buffer(size);

	HAKU_SOK_ASSERT(Device->get()->CreateCommittedResource(
		&uploadheapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadbuffer)));

	HAKU_DXNAME(uploadbuffer, L"Vertex Upload Buffer")

	UINT8*		  pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	HAKU_SOK_ASSERT(uploadbuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, ptr, size);

	D3D12_SUBRESOURCE_DATA bufferData{};
	bufferData.pData	  = pVertexDataBegin;
	bufferData.RowPitch	  = size;
	bufferData.SlicePitch = size;
	UpdateSubresources(CommandQueue->GetCommandList(), m_Resource.Get(), uploadbuffer.Get(), 0, 0, 1, &bufferData);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	CommandQueue->GetCommandList()->ResourceBarrier(1, &barrier);
	uploadbuffer->Unmap(0, nullptr);

	m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes  = sizeof(VertexData);
	m_VertexBufferView.SizeInBytes	  = size;
	HAKU_DXNAME(m_Resource, L"Vertex_Buffer")

	CommandQueue->Close();
	CommandQueue->Execute();
	CommandQueue->Synchronize();
}
D3D12VertexBuffer::~D3D12VertexBuffer()
{
	HAKU_LOG_INFO("Deleting constant buffer");
}
void D3D12VertexBuffer::SetBuffer(Haku::Renderer::D3D12CommandQueue* CommandQueue) noexcept
{
	CommandQueue->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBufferView);
}
D3D12ConstBuffer::D3D12ConstBuffer(Haku::Renderer::D3D12RenderDevice* Device, ID3D12DescriptorHeap* Heap)
	: D3D12Resource(Device, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD, sizeof(ConstData))
{
	m_Type = D3D12RESOURCE_TYPE::CONSTANT_RESOURCE;
	Data.Matrix =
		DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity() * DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.6f));
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resdesc  = CD3DX12_RESOURCE_DESC::Buffer(sizeof(ConstData));
	auto size	  = sizeof(ConstData);
	// Describe and create a constant buffer view.
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation					= m_Resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes						= size;
	auto cpuhandle							= Heap->GetCPUDescriptorHandleForHeapStart();
	Device->get()->CreateConstantBufferView(&cbvDesc, cpuhandle);
	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	HAKU_SOK_ASSERT(m_Resource->Map(0, &readRange, reinterpret_cast<void**>(&m_ptr)));
	memcpy(m_ptr, &Data, sizeof(Data));
	m_Resource->SetName(L"Constant buffer");
	HAKU_LOG_INFO("creating const buffer");
	HAKU_DXNAME(m_Resource, L"Constant buffer")
}

D3D12ConstBuffer::~D3D12ConstBuffer()
{
	HAKU_LOG_INFO("deleted const buffer");
	m_ptr = nullptr;
}

void D3D12ConstBuffer::Update(DirectX::XMMATRIX& ref) noexcept
{
	Data.Matrix = ref;
	memcpy(m_ptr, &Data, sizeof(Data));
}

void D3D12ConstBuffer::Update(float* rotate, float* translate, float width, float height) noexcept
{
	Data.Matrix = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(rotate[0]) * DirectX::XMMatrixRotationY(rotate[1]) *
		DirectX::XMMatrixRotationZ(rotate[2]) *
		DirectX::XMMatrixTranslation(translate[0], translate[1], translate[2] + 0.5f) *
		DirectX::XMMatrixPerspectiveLH(width / width, height / width, 0.1f, 10.00f));
	memcpy(m_ptr, &Data, sizeof(Data));
}

void D3D12ConstBuffer::SetBuffer(D3D12CommandQueue* CommandQueue, UINT slot)
{
	// ConstBuffer view must be auto postion.
	CommandQueue->GetCommandList()->SetGraphicsRootConstantBufferView(slot, m_Resource->GetGPUVirtualAddress());
}

D3D12Resource::D3D12Resource(
	Haku::Renderer::D3D12RenderDevice* Device,
	D3D12_RESOURCE_STATES			   state,
	D3D12_HEAP_TYPE					   type,
	size_t							   size,
	D3D12_HEAP_FLAGS				   flags)
{
	auto defheapprop = CD3DX12_HEAP_PROPERTIES(type);
	auto resdesc	 = CD3DX12_RESOURCE_DESC::Buffer(size);
	HAKU_SOK_ASSERT(Device->get()->CreateCommittedResource(
		&defheapprop, flags, &resdesc, state, nullptr, IID_PPV_ARGS(&m_Resource)));
}

D3D12Resource::~D3D12Resource()
{
	m_Resource.Reset();
}

D3D12TextureBuffer::D3D12TextureBuffer(
	D3D12RenderDevice*		 Device,
	D3D12CommandQueue*		 CommandQueue,
	D3D12DescriptorHeap*	 Heap,
	uint8_t*				 resource_ptr,
	uint64_t				 slot,
	D3D12_RESOURCE_DIMENSION dimension,
	uint64_t				 num_subresource,
	uint64_t				 height,
	uint64_t				 width,
	uint64_t				 depth,
	D3D12_RESOURCE_FLAGS	 flags)
	:D3D12Resource(Device, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT, 1)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> texture_uploadheap;

	m_Type = D3D12RESOURCE_TYPE::TEXTURE_RESOURCE;
	D3D12_RESOURCE_DESC TexDesc{};
	TexDesc.MipLevels		   = 1;
	TexDesc.DepthOrArraySize   = 1;
	TexDesc.SampleDesc.Count   = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Width			   = width;
	TexDesc.Height			   = height;
	TexDesc.Flags			   = flags;
	TexDesc.Format			   = DXGI_FORMAT_B8G8R8A8_UNORM;
	TexDesc.Dimension		   = dimension;

	auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//HAKU_SOK_ASSERT(Device->get()->CreateCommittedResource(
	//	&heap_prop, D3D12_HEAP_FLAG_NONE, &TexDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Resource)))
	auto uploadBuffersize = GetRequiredIntermediateSize(m_Resource.Get(), 0, num_subresource);

	auto upload_heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto uploadresdesc	 = CD3DX12_RESOURCE_DESC::Buffer(uploadBuffersize);
	HAKU_SOK_ASSERT(Device->get()->CreateCommittedResource(
		&upload_heapprop,
		D3D12_HEAP_FLAG_NONE,
		&uploadresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture_uploadheap)))

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData				   = resource_ptr;
	textureData.RowPitch			   = width;
	textureData.SlicePitch			   = textureData.RowPitch * height;
	//UpdateSubresources(CommandQueue->GetCommandList(), m_Resource, texture_uploadheap, 0, 0, 1, &textureData);
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	CommandQueue->GetCommandList()->ResourceBarrier(1, &barrier);
	
	m_TextureView.Texture2D.MipLevels	  = 1;
	m_TextureView.Format				  = TexDesc.Format;
	m_TextureView.ViewDimension			  = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_TextureView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(Heap->GetSRVCPUHandle());

	rtvHandle.Offset(slot, Device->get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	Device->get()->CreateShaderResourceView(m_Resource.Get(), &m_TextureView, rtvHandle);

	HAKU_DXNAME(m_Resource, L"Texture Resource")
	Microsoft::WRL::ComPtr<ID3D12DebugCommandList> Check;
	CommandQueue->GetCommandList()->QueryInterface(IID_PPV_ARGS(&Check));
}

} // namespace Renderer
} // namespace Haku
