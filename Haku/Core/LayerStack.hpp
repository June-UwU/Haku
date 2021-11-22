#include "../macros.hpp"
#include "Events.hpp"
#include "Layer.hpp"
#include "../hakupch.hpp"

namespace Haku
{
class HAKU_API LayerStack
{
	using Stack = std::vector<Layer*>;

public:
	~LayerStack();
	LayerStack() = default;
	bool OnEvent(Event& event);
	void Addlayer(Layer* layer) noexcept;
	void AddOverLay(Layer* layer) noexcept;
	void RemoveLayer(Layer* layer) noexcept;
	void RemoveOverlay(Layer* layer) noexcept;

private:
	Stack m_LayerStack;
	Stack m_OverLayStack;
};
} // namespace Haku
