#pragma once
#include "../../macros.hpp"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "VertexStruct.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12DescriptorHeaps.hpp"
namespace Haku
{
namespace Renderer
{
enum D3D12RESOURCE_TYPE
{
	GENERIC_RESOURCE = 0, // this shouldn't exist
	INDEX_RESOURCE,
	VERTEX_RESOURCE,
	TEXTURE_RESOURCE,
	CONSTANT_RESOURCE
};
class D3D12Resource
{
	// should I need to make this public and constructable
protected:
	D3D12Resource(
		Haku::Renderer::D3D12RenderDevice* Device,
		D3D12_RESOURCE_STATES			   state,
		D3D12_HEAP_TYPE					   type,
		size_t							   size,
		D3D12_HEAP_FLAGS				   flags = D3D12_HEAP_FLAG_NONE);
	~D3D12Resource();

	Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
	D3D12RESOURCE_TYPE					   m_Type = D3D12RESOURCE_TYPE::GENERIC_RESOURCE;
};

class D3D12VertexBuffer : public D3D12Resource
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
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

class D3D12ConstBuffer : public D3D12Resource
{
public:
	D3D12ConstBuffer(Haku::Renderer::D3D12RenderDevice* Device, ID3D12DescriptorHeap* UI_Desciptor);
	~D3D12ConstBuffer();
	void Update(DirectX::XMMATRIX& ref) noexcept;
	void Update(float* rotate, float* translate, float width, float height) noexcept;
	void SetBuffer(D3D12CommandQueue* CommandQueue, UINT slot);

private:
	uint8_t*  m_ptr;
	ConstData Data{};
};

class D3D12TextureBuffer : public D3D12Resource
{
public:
	D3D12TextureBuffer(
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
		D3D12_RESOURCE_FLAGS	 flags);

private:
	CD3DX12_CPU_DESCRIPTOR_HANDLE	m_Handle;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_TextureView;
};

} // namespace Renderer
} // namespace Haku
