#pragma once
#include "..\macros.hpp"
#include "..\hakupch.hpp"

class Event;
namespace Haku
{
class HAKU_API UILayer
{
public:
	UILayer()		   = default;
	virtual ~UILayer() = default;
	virtual void Render(){};
	virtual void OnAttach(){};
	virtual void OnDetach(){};
	// virtual void OnEvent(Event& ref) = 0;
};
} // namespace Haku
