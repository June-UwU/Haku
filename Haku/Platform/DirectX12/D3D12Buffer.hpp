#pragma once
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "../../Renderer/Buffer.hpp"
#include "../../Renderer/VertexStruct.hpp"

namespace Haku
{
namespace Renderer
{
class D3D12VertexBuffer : public VertexBuffer
{
public:
	D3D12VertexBuffer(
		Haku::Renderer::D3D12RenderDevice& Device,
		Haku::Renderer::D3D12CommandQueue& CommandQueue,
		VertexData*						   ptr,
		size_t							   size);
	void SetBuffer(Haku::Renderer::D3D12CommandQueue& CommandQueue) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			   m_VertexBufferView;
};
} // namespace Renderer
} // namespace Haku
