#pragma once

#include "macros.hpp"
#include "hakupch.hpp"
#include "HakuLog.hpp"
#include "directx/d3d12.h"
#include "D3D12Resources.hpp"
#include "VertexProperties.hpp"

namespace Haku
{
namespace Renderer
{
class CommandList;

//TODO : expand the upload buffer

struct VertexLoadTable
{
	ID3D12Resource*		   Destination_ptr;
	uint64_t			   offset; // in count should be multipled with the size of VertexData
	D3D12_SUBRESOURCE_DATA subresourcedata;
};
class UploadBuffer
{
public:
	UploadBuffer(uint32_t buffersize = 2 * MiB);
	DISABLE_COPY(UploadBuffer)
	DISABLE_MOVE(UploadBuffer)
	~UploadBuffer();

	ID3D12Resource*				  Get() noexcept;
	std::vector<VertexData>		  GetDataVector();
	D3D12_GPU_VIRTUAL_ADDRESS	  GetBaseAddress();
	uint32_t					  GetBufferOffSet();
	void						  Release() noexcept;
	std::unique_ptr<VertexBuffer> MakeVertexBuffer();
	bool						  HasBuffer() noexcept;
	void						  IncrementDataOffset() noexcept;
	void						  AddVertexLoadTable(VertexLoadTable loadtable);
	void						  TransitionVertexBuffers(CommandList* commandlist);
	void						  SubmitVertex(const std::vector<VertexData> vertex) noexcept;
	void						  SubmitVertex(const VertexData* data, uint64_t count) noexcept;
	void AddVertexLoadTable(ID3D12Resource* Destination_ptr, uint64_t offset, D3D12_SUBRESOURCE_DATA subresourcedata);

private:
	std::vector<uint64_t>				 m_Offset;
	ID3D12Resource*						 m_Resource;
	std::vector<VertexLoadTable>		 m_LoadTable;
	uint32_t							 m_UploadPos;
	uint32_t							 m_BufferSize;
	std::vector<D3D12_RESOURCE_BARRIER>	 m_BarrierList;
	D3D12_GPU_VIRTUAL_ADDRESS			 m_BaseAddress;
	std::vector<std::vector<VertexData>> m_VertexHolder;
	uint32_t							 m_AllocatedBuffer;
};
} // namespace Renderer
} // namespace Haku
