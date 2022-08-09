#ifdef HAKU_API_BUILD
#define HAPI __declspec(dllexport)
#else
#define HAPI __declspec(dllimport)
#endif
