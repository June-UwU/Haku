/*****************************************************************//**
 * \file   engine_shaders.hpp
 * \brief  engine specfic shader
 * 
 * \author June
 * \date   November 2022
 *********************************************************************/
#include "defines.hpp"
#include "directx_types.INL"


typedef enum ENGINE_SHADER
{
	FULL_TRIANGLE_VERTEX_SHADER = 0x0,
	
	FULL_TRIANGLE_PIXEL_SHADER,

	ENGINE_SHADER_MAX
}ENGINE_SHADER;


constexpr const char* ENGINE_SHADER_NAME[ENGINE_SHADER_MAX]
{
	"full triangle vertex shader"
	"full triangle pixel shader"
};

ID3D10Blob* get_shader(ENGINE_SHADER shader);

D3D12_SHADER_BYTECODE get_byte_code(ENGINE_SHADER shader);

i8 initialize_engine_shader(void);

void shutdown_engine_shader(void);
