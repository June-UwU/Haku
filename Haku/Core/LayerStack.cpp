#include "LayerStack.hpp"

namespace Haku
{
LayerStack::~LayerStack()
{
	for (Layer* layer : m_LayerStack)
	{
		delete layer;
	}
	for (Layer* layer : m_OverLayStack)
	{
		delete layer;
	}
}
bool LayerStack::OnEvent(Event& event)
{
	bool ret = false;
	for (Layer* layer : m_OverLayStack)
	{
		ret = layer->OnEvent(event);
		if (ret)
		{
			return ret;
		}
	}
	for (Layer* layer : m_LayerStack)
	{
		ret = layer->OnEvent(event);
		if (ret)
		{
			return ret;
		}
	}
	return ret;
}
void LayerStack::Addlayer(Layer* layer) noexcept
{
	m_LayerStack.emplace_back(layer);
}
void LayerStack::AddOverLay(Layer* layer) noexcept
{
	layer->OnAttach();
	m_LayerStack.emplace_back(layer);
}
void LayerStack::RemoveLayer(Layer* layer) noexcept
{
	auto pos = std::find(m_LayerStack.begin(), m_LayerStack.end(), layer);
	if (pos != m_LayerStack.end())
	{
		layer->OnDetach();
		m_LayerStack.erase(pos);
	}
}

void LayerStack::RemoveOverlay(Layer* layer) noexcept
{
	auto pos = std::find(m_OverLayStack.begin(), m_OverLayStack.end(), layer);
	if (pos != m_OverLayStack.end())
	{
		layer->OnDetach();
		m_OverLayStack.erase(pos);
	}
}

} // namespace Haku
