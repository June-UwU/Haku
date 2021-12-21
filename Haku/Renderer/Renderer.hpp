#pragma once
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "wil/wrl.h"
#include "VertexStruct.hpp"
// Note that while Microsoft::WRL::ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().

namespace Haku
{
namespace Renderer
{
class Renderer
{
public:
	Renderer() = default;
	Renderer(uint32_t height, uint32_t width)
		: height(height)
		, width(width)
	{
	}
	virtual void Cleanup() = 0;
	virtual void Render()  = 0;
	virtual void Update()  = 0;
	// virtual void Init(Window* window) = 0;
	uint32_t getHeight() { return height; }
	uint32_t getWidth() { return width; }

protected:
	uint32_t height = 0;
	uint32_t width	= 0;
};
} // namespace Renderer
} // namespace Haku
