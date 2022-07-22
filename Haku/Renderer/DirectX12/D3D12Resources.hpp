#pragma once

#include "macros.hpp"
#include "HakuLog.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"

namespace Haku
{
namespace Renderer
{
class UploadBuffer;

class D3DResource
{
public:
	D3DResource();
	D3DResource(D3DResource&& rhs);
	D3DResource(const D3DResource& rhs);

	D3DResource& operator=(D3DResource&& rhs);
	D3DResource& operator=(const D3DResource& rhs);

	~D3DResource();

	ID3D12Resource* Get() noexcept;
	void			Release() noexcept;

protected:
	ID3D12Resource* m_Resource;
};

class VertexBuffer : public D3DResource
{
public:
	VertexBuffer(uint64_t buffersize);
	VertexBuffer(const VertexBuffer& rhs);
	VertexBuffer(VertexBuffer&& rhs);
	VertexBuffer& operator=(const VertexBuffer& rhs);
	VertexBuffer& operator=(VertexBuffer&& rhs);

	D3D12_VERTEX_BUFFER_VIEW* GetView() noexcept;
	void					  Initialize(UploadBuffer& uploadbuffer);

private:
	D3D12_VERTEX_BUFFER_VIEW m_View;
};
} // namespace Renderer
} // namespace Haku
