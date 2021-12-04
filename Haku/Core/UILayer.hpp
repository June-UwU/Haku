#pragma once
#include "..\macros.hpp"
#include "..\hakupch.hpp"

class Event;
namespace Haku
{
class HAKU_API UILayer
{
public:
	UILayer() = default;
	void		 OnAttach();
	void		 Render();
	void		 OnDetach();
	//virtual void OnEvent(Event& ref) = 0;
};
} // namespace Haku
