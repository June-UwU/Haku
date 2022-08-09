#include <cstdint>


//HAKU TYPE DEFINES


//signed types 

#define 	i8 	int8_t
#define 	i16 	int16_t
#define 	i32	int32_t
#define 	i64	int64_t

//unsigned type

#define 	u8	uint8_t
#define 	u16	uint16_t
#define 	u32	uint32_t
#define 	u64	uint32_t

//floating point types

#define 	f32 	float
#define 	f64 	double




//HAKU API EXPORT MACROS

#ifdef HAKU_API_BUILD
#define HAPI __declspec(dllexport)
#else
#define HAPI __declspec(dllimport)
#endif
