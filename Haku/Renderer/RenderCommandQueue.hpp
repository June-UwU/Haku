#pragma once

namespace Haku
{
namespace Renderer
{
class RenderCommandQueue
{
public:
	RenderCommandQueue()	   = default;
	virtual void Close()	   = 0;
	virtual void Execute()	   = 0;
	virtual void Synchronize() = 0;

private:
};
} // namespace Renderer
} // namespace Haku
