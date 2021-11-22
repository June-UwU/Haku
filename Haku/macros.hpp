#include <cstdint>
#ifdef HAKU_API_BUILD
#define HAKU_API __declspec(dllexport)
#else
#define HAKU_API __declspec(dllimport)
#endif

#define BIT(x) (1 << x)

#define HAKU_BIND_FUNC(x)                                                                                              \
	[this](auto&&... args) -> decltype(auto)                                                                           \
	{                                                                                                                  \
		return this->x(std::forward<decltype(args)>(args)...);                                                         \
	}

#define HAKU_ASSERT(x)                                                                                                 \
	if (!x)                                                                                                            \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		__debugbreak();                                                                                                \
	}

#ifdef _DEBUG
#define HAKU_IF_CHECK_ASSERT(x)                                                                                        \
	if (!x)                                                                                                            \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		__debugbreak();                                                                                                \
	}
#elif
/*we need a throwable*/
#define HAKU_IF_CHECK_ASSERT(x)
#endif
