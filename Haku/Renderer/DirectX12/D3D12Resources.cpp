#include "D3D12Renderer.hpp"
#include "D3D12Resources.hpp"
#include "D3D12UploadBuffer.hpp"

namespace Haku
{
namespace Renderer
{
D3DResource::D3DResource()
{
	m_Resource = nullptr;
}
D3DResource::D3DResource(D3DResource&& rhs)
	: m_Resource(std::move(rhs.m_Resource))
{
}
D3DResource::D3DResource(const D3DResource& rhs)
	: m_Resource(rhs.m_Resource)
{
}
D3DResource& D3DResource::operator=(D3DResource&& rhs)
{
	m_Resource = std::move(rhs.m_Resource);
	return *this;
}
D3DResource& D3DResource::operator=(const D3DResource& rhs)
{
	m_Resource = rhs.m_Resource;
	return *this;
}
D3DResource::~D3DResource()
{
	Release();
}

void D3DResource::Release() noexcept
{
	if (m_Resource)
	{
		m_Resource->Release();
	}
}

ID3D12Resource* D3DResource::Get() noexcept
{
	return m_Resource;
}

VertexBuffer::VertexBuffer(uint64_t buffersize)
{
	auto Device	  = RenderEngine::GetDeviceD3D();
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resdesc  = CD3DX12_RESOURCE_DESC::Buffer(buffersize, D3D12_RESOURCE_FLAG_NONE);
	auto sok	  = Device->CreateCommittedResource(
		 &heapprop,
		 D3D12_HEAP_FLAG_NONE,
		 &resdesc,
		 D3D12_RESOURCE_STATE_COPY_DEST,
		 nullptr,
		 __uuidof(ID3D12Resource),
		 reinterpret_cast<void**>(&m_Resource));
	HAKU_SOK_ASSERT(sok)

	
	m_View.BufferLocation = m_Resource->GetGPUVirtualAddress();
	m_View.SizeInBytes	  = buffersize;
	m_View.StrideInBytes  = sizeof(VertexData);
}

VertexBuffer::VertexBuffer(const VertexBuffer& rhs)
	: D3DResource(rhs)
{
	m_Resource = rhs.m_Resource;
	m_View	   = rhs.m_View;
}

VertexBuffer::VertexBuffer(VertexBuffer&& rhs)
	: D3DResource(rhs)
{
	m_Resource = rhs.m_Resource;
	m_View	   = m_View;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& rhs)
{
	m_Resource = rhs.m_Resource;
	m_View	   = rhs.m_View;
	return *this;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs)
{
	m_Resource = rhs.m_Resource;
	m_View	   = rhs.m_View;
	return *this;
}

D3D12_VERTEX_BUFFER_VIEW* VertexBuffer::GetView() noexcept
{
	return &m_View;
}

void VertexBuffer::Initialize(UploadBuffer& uploadbuffer)
{
	auto		  ImmediateBuffer = uploadbuffer.Get();
	auto		  Offset		  = uploadbuffer.GetBufferOffSet();
	auto		  bufferdata	  = uploadbuffer.GetDataVector();
	auto		  buffersize	  = bufferdata.size() * sizeof(VertexData);
	uint8_t*	  ptr;
	CD3DX12_RANGE readrange(0, 0);
	HAKU_SOK_ASSERT(ImmediateBuffer->Map(0, &readrange, reinterpret_cast<void**>(&ptr)))
	memcpy(ptr, bufferdata.data(), buffersize);
	ImmediateBuffer->Unmap(0, nullptr);

	D3D12_SUBRESOURCE_DATA subresourcedata{};
	subresourcedata.pData	   = ptr;
	subresourcedata.SlicePitch = buffersize;
	subresourcedata.RowPitch   = buffersize;
	// make upload buffer table and do a command list for all together
	// things needed for the table
	// commandlist
	// destination resource ptr
	// source and offset offset should be in bytes ..?
	// subresource data

	uploadbuffer.AddVertexLoadTable(m_Resource, Offset, subresourcedata);
	uploadbuffer.IncrementDataOffset();
}

} // namespace Renderer
} // namespace Haku
