#pragma once

namespace Haku
{
namespace Renderer
{
class RenderDevice
{
public:
	RenderDevice() = default;
	virtual void Render() = 0;
private:
};
} // namespace Renderer
} // namespace Haku
