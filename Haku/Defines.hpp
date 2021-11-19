#include <cstdint>
#include <string>
#ifdef HAKU_API_BUILD
#define HAKU_API __declspec(dllexport)
#else
#define HAKU_API __declspec(dllimport)
#endif

#define BIT(x) (1 << x)
