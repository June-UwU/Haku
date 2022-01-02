#pragma once

/*MAKE THE EVENT SYSTEM AN COMMON CLASS WITH EXTRACTABLE PARAM AND BIT SET FOR APPICATION EVENT TYPE*/
#include "HakuLog.hpp"
#include "../macros.hpp"
#include "../hakupch.hpp"

/*Things Needed For The Event System Event Entity */
// Mouse Clicks Handled By Eventtypes
// Keyboard Strokes Handled By Eventtypes

// What Needs To Be Handled
// The Mouse Position As 32 Bit Value

namespace Haku
{
#define HAKU_EVENT_OR(x, y) static_cast<uint32_t>(static_cast<uint32_t>(x) | static_cast<uint32_t>(y))
/*MACROS TO CREATE ,PACK AND EXRACT HAKU EVENTS*/
#define HAKU_EVENT_TYPE(x)	(x & 0xFFFFFFF0)
/*mouse event pack macro*/
#define HAKU_MOUSE_PACK(x, y, z)                                                                                       \
	x = y;                                                                                                             \
	x = (x << 32) | z;

/*mouse event extract macro*/
#define HAKU_MOUSE_EXTRACT(x, y, z)                                                                                    \
	y = 0xFFFF & (x >> 32);                                                                                            \
	z = 0xFFFF & x;
#define HAKU_DISPLAY_SIZE_EXTRACT(param, height, width)                                                                \
	width  = 0xFFFF & param;                                                                                           \
	height = 0xFFFF & (param >> 32);

#define HAKU_EVENT(x, y, z)		Event(static_cast<uint32_t>(x) | static_cast<uint32_t>(y), z)
#define HAKU_APP_EVENT(x, y)	Event(static_cast<uint32_t>(EventCatagory::ApplicationEvent) | static_cast<uint32_t>(x), y)
#define HAKU_KEY_EVENT(x, y)	Event(static_cast<uint32_t>(EventCatagory::KeyBoard) | static_cast<uint32_t>(x), y)
#define HAKU_MOUSE_EVENT(x, y)	Event(static_cast<uint32_t>(EventCatagory::MouseEvent) | static_cast<uint32_t>(x), y)
#define HAKU_WINDOW_EVENT(x, y) Event(static_cast<uint32_t>(EventCatagory::WindowEvent) | static_cast<uint32_t>(x), y)

enum class EventCatagory : uint32_t
{
	ApplicationEvent =
		BIT(0), // Application Event That is indendented to act as the control for the Dispatcher service routine
	WindowEvent = BIT(1),
	KeyBoard	= BIT(2),
	MouseEvent	= BIT(3),
};
/*might need to add more*/

enum class EventType : uint32_t
{
	WindowMaximizeEvent		= BIT(4),
	WindowMinimizeEvent		= BIT(5),
	WindowResizeEvent		= BIT(6),
	WindowCloseEvent		= BIT(7),
	KeyUpEvent				= BIT(8),
	KeyDownEvent			= BIT(9),
	MouseLeftDownEvent		= BIT(10),
	MouseRightDownEvent		= BIT(11),
	MouseMiddleDownEvent	= BIT(12),
	MouseLeftUpEvent		= BIT(13),
	MouseRightUpEvent		= BIT(14),
	MouseMiddleUpEvent		= BIT(15),
	MouseScrollUpEvent		= BIT(16),
	MouseScrollDownEvent	= BIT(17),
	MouseMoveEvent			= BIT(18),
	ApplicationTickEvent	= BIT(19),
	AppicationRenderEvent	= BIT(20),
	ApplicationUpdateEvent	= BIT(21),
	ApplicationSuspendEvent = BIT(22),
	None					= BIT(23),
	Recurring				= BIT(30)
};

class HAKU_API Event
{
public:
	~Event() = default;
	Event(uint32_t bits, int64_t data)
		: EventBitSet(bits)
		, Data(data)
	{
		HAKU_LOG_INFO("Event :", bits, "Data :", data);
	}
	int64_t	  GetData() { return Data; }
	uint32_t  GetEvent() { return EventBitSet; }
	EventType GetEventType() { return (static_cast<EventType>(HAKU_EVENT_TYPE(EventBitSet))); }

public:
	bool Handled = false;

private:
	int64_t	 Data;
	uint32_t EventBitSet;
};

// should handle the the function as the dispatcher internal structure of functions
// Skittles tip : change the EventQueue to a std::vector...? might as well do it with the logging support

class EventDispatcher
{
	using EventQueue  = std::list<Event>;
	using Routine	  = std::function<void(Event& e)>;
	using CallBackMap = std::unordered_map<uint32_t, Routine>;

public:
	EventDispatcher() = default;
	void   ServiceEvent();
	Event& peek() { return m_Queue.front(); }
	bool   empty() { return m_Queue.empty(); }
	void   RegisterRoutine(Routine r, uint32_t type);
	void   OnEvent(Event& e) noexcept { m_Queue.push_back(e); }

private:
	EventQueue	m_Queue; /// Looks like std::queue is out
	CallBackMap m_CallBack;
};

} // namespace Haku
