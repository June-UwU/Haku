#include "D3D12UploadBuffer.hpp"
#include "D3D12Renderer.hpp"
#include "D3D12CommandList.hpp"
namespace Haku
{
namespace Renderer
{
UploadBuffer::UploadBuffer(uint32_t buffersize)
	: m_BufferSize(buffersize)
	, m_AllocatedBuffer(0)
	, m_UploadPos(0)
{
	auto Device		= RenderEngine::GetDeviceD3D();
	auto upheapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resdesc	= CD3DX12_RESOURCE_DESC::Buffer(buffersize, D3D12_RESOURCE_FLAG_NONE);
	auto sok		= Device->CreateCommittedResource(
		   &upheapprop,
		   D3D12_HEAP_FLAG_NONE,
		   &resdesc,
		   D3D12_RESOURCE_STATE_GENERIC_READ,
		   nullptr,
		   __uuidof(ID3D12Resource),
		   reinterpret_cast<void**>(&m_Resource));
	m_BaseAddress = m_Resource->GetGPUVirtualAddress();
	HAKU_SOK_ASSERT(sok);
}
UploadBuffer::~UploadBuffer()
{
	Release();
}
ID3D12Resource* UploadBuffer::Get() noexcept
{
	return m_Resource;
}
void UploadBuffer::Release() noexcept
{
	if (m_Resource)
	{
		m_Resource->Release();
	}
}

void UploadBuffer::TransitionVertexBuffers(CommandList* commandlist)
{
	for (size_t i = 0; i < m_LoadTable.size(); i++)
	{
		auto table = m_LoadTable[i];
		UpdateSubresources(
			commandlist->m_CommandList,
			table.Destination_ptr,
			m_Resource,
			table.offset * sizeof(VertexData),
			0,
			1,
			&table.subresourcedata);
		m_BarrierList.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			table.Destination_ptr,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}
	commandlist->m_CommandList->ResourceBarrier(m_BarrierList.size(), m_BarrierList.data());
}
void UploadBuffer::SubmitVertex(const std::vector<VertexData> vertex) noexcept
{
	m_Offset.push_back(m_AllocatedBuffer);
	m_VertexHolder.push_back(vertex);
	m_AllocatedBuffer += m_VertexHolder.size();
}
void UploadBuffer::SubmitVertex(const VertexData* data, uint64_t count) noexcept
{
	m_Offset.push_back(m_AllocatedBuffer);
	m_AllocatedBuffer += count * sizeof(VertexData);
	std::vector<VertexData> push_vector;
	for (size_t i = 0; i < count; i++)
	{
		push_vector.push_back(data[i]);
	}
	m_VertexHolder.push_back(push_vector);
}
uint32_t UploadBuffer::GetBufferOffSet()
{
	return m_Offset[m_UploadPos];
}
std::vector<VertexData> UploadBuffer::GetDataVector()
{
	auto ret_val = m_VertexHolder[m_UploadPos];
	return ret_val;
}
D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer::GetBaseAddress()
{
	return m_BaseAddress;
}
void UploadBuffer::IncrementDataOffset() noexcept
{
	m_UploadPos++;
}
void UploadBuffer::AddVertexLoadTable(
	ID3D12Resource*		   Destination_ptr,
	uint64_t			   offset,
	D3D12_SUBRESOURCE_DATA subresourcedata)
{
	VertexLoadTable load_table{};
	load_table.Destination_ptr = Destination_ptr;
	load_table.offset		   = offset;
	load_table.subresourcedata = subresourcedata;

	m_LoadTable.push_back(load_table);
}
void UploadBuffer::AddVertexLoadTable(VertexLoadTable loadtable)
{
	m_LoadTable.push_back(loadtable);
}
std::unique_ptr<VertexBuffer> UploadBuffer::MakeVertexBuffer()
{
	// UpdateSubresources(commandlist->m_CommandList, m_Resource, ImmediateBuffer, Offset, 0, 1, &subresourcedata);
	// CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//	m_Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	// commandlist->m_CommandList->ResourceBarrier(1, &barrier);

	uint64_t buffersize = m_VertexHolder[m_UploadPos].size() * sizeof(VertexData);
	auto	 ret		= std::make_unique<VertexBuffer>(buffersize);
	ret->Initialize(*this);
	// stud
	return ret;
}
bool UploadBuffer::HasBuffer() noexcept
{
	if (m_UploadPos == m_VertexHolder.size())
	{
		return false;
	}
	return true;
}
} // namespace Renderer
} // namespace Haku
