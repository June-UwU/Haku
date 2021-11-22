#pragma once
#include "../macros.hpp"
#include "Events.hpp"
#include "../hakupch.hpp"

namespace Haku
{
// Do we need a debug name...?
class HAKU_API Layer
{
public:
	Layer(const std::string name = "Layer")
		: m_DebugName(name)
	{
	}
	virtual ~Layer() = default;
	virtual void	   OnAttach(){};
	virtual void	   OnDetach(){};
	virtual bool	   OnEvent(Event& event) { HAKU_LOG_INFO(__FUNCTION__); return false; };
	const std::string& GetName() { return m_DebugName; }

private:
	std::string m_DebugName;
};
} // namespace Haku
