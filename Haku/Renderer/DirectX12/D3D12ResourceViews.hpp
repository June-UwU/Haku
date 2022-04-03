#pragma once
#include "../../macros.hpp"
#include "D3D12Resources.hpp"
#include "D3D12CommandQueue.hpp"

namespace Haku
{
namespace Renderer
{
// this might be later reworked to make this a hash structure and have this as a high level interface
enum class D3D12VIEWTYPE
{
	VERTEX_VIEW	  = BIT(0),
	INDEX_VIEW	  = BIT(1),
	TEXTURE_VIEW  = BIT(2),
	CONSTANT_VIEW = BIT(3)
};

class ResourceView
{
public:
	virtual void SetResource(Haku::Renderer::D3D12CommandQueue Command_Queue, size_t slot) = 0;

protected:
	ResourceView(D3D12VIEWTYPE type)
		: m_Type(type){};
	~ResourceView(){};
	D3D12VIEWTYPE m_Type;
};

class VertexResourceView : public ResourceView
{
public:
	VertexResourceView()
		: ResourceView(D3D12VIEWTYPE::VERTEX_VIEW){};
	void SetResource(Haku::Renderer::D3D12CommandQueue Command_Queue, size_t slot){};

private:
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};
} // namespace Renderer
} // namespace Haku
