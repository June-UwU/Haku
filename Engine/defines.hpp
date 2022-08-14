#pragma once
#include <cstdlib>
#include <cstdint>

// TODO : make an haku specfic asserts

//HAKU TYPE DEFINES

#define HASSERT(condition) if(!condition)\
{\
	HLEMER("Assertion Failure..!\n File : %s \n Line : %d",__FILE__ , __LINE__);\
	exit(-1);\
}

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

//HAKU RETURN CODES

#define 	H_OK	0
#define 	H_FAIL	-1


//HAKU API EXPORT MACROS

#ifdef HAKU_API_BUILD
#define HAPI __declspec(dllexport)
#else
#define HAPI __declspec(dllimport)
#endif
