#pragma once
#include <cstdlib>
#include <cstdint>

//HAKU TYPE DEFINES

#define HASSERT(condition) if(!condition)\
{\
	HLEMER("Assertion Failure..!\n File : %s \n Line : %d",__FILE__ , __LINE__);\
	exit(-1);\
}


// objects that are intended to be opaque

#define 	opaque	void


//signed types 

#define 	i8 	int8_t
#define 	i16 	int16_t
#define 	i32	int32_t
#define 	i64	int64_t

//unsigned type

#define 	u8	uint8_t
#define 	u16	uint16_t
#define 	u32	uint32_t
#define 	u64	uint64_t

//floating point types

#define 	f32 	float
#define 	f64 	double

//HAKU RETURN CODES

constexpr i8 	H_OK	= 0 ;
constexpr i8 	H_FAIL	= -1;
constexpr i8	H_ERR	= -2;

constexpr u64 BYTESINKiB(u64 scale)
{
	return 1024 * scale;
}

constexpr u64 BYTESINMiB(u64 scale)
{
	return 1024 * 1024 * scale;
}
constexpr u64 BYTESINGiB(u64 scale)
{
	return 1024 * 1024 * 1024 * scale;
}

constexpr u64 BYTESTOKiB(u64 bytes)
{
	return bytes / 1024u;
}

constexpr u64 BYTESTOMiB(u64 bytes)
{
	return BYTESTOKiB(bytes) / 1024u;
}

constexpr u64 BYTESTOGiB(u64  bytes)
{
	return BYTESTOMiB(bytes) / 1024u;
}
//HAKU API EXPORT MACROS

#ifdef HAKU_API_BUILD
#define HAPI __declspec(dllexport)
#else
#define HAPI __declspec(dllimport)
#endif
