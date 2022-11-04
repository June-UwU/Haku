/*****************************************************************//**
 * \file   engine_shaders.cpp
 * \brief  
 * 
 * \author June
 * \date   November 2022
 *********************************************************************/
#include "engine_shaders.hpp"
#include "shader_compiler.hpp"
#include "core/logger.hpp"

static ID3D10Blob* SHADER_STORE[ENGINE_SHADER_MAX];

ID3D10Blob* get_shader(ENGINE_SHADER shader)
{
	return SHADER_STORE[shader];
}

D3D12_SHADER_BYTECODE get_byte_code(ENGINE_SHADER shader)
{
	
	D3D12_SHADER_BYTECODE rv{};

	rv.BytecodeLength = SHADER_STORE[shader]->GetBufferSize();
	rv.pShaderBytecode = SHADER_STORE[shader]->GetBufferPointer();

	return rv;
}

i8 initialize_engine_shader(void)
{
	SHADER_STORE[FULL_TRIANGLE_VERTEX_SHADER] = create_shader_byte_code(L"D:\\Haku\\bin\\BuiltIn_Vertex.cso", HK_VERTEX_SHADER);
	if (nullptr == SHADER_STORE[FULL_TRIANGLE_VERTEX_SHADER])
	{
		HLEMER("failed to get engine shader \n \t : %s ", ENGINE_SHADER_NAME[FULL_TRIANGLE_VERTEX_SHADER]);
		return H_FAIL;
	}
	SHADER_STORE[FULL_TRIANGLE_PIXEL_SHADER] = create_shader_byte_code(L"D:\\Haku\\bin\\BuiltIn_Pixel.cso", HK_PIXEL_SHADER);
	if (nullptr == SHADER_STORE[FULL_TRIANGLE_PIXEL_SHADER])
	{
		HLEMER("failed to get engine shader \n \t : %s", ENGINE_SHADER_NAME[FULL_TRIANGLE_PIXEL_SHADER]);
		return H_FAIL;
	}

	return H_OK;
}

void shutdown_engine_shader(void)
{
	for (u64 i = 0; i < ENGINE_SHADER_MAX; i++)
	{
		SHADER_STORE[i]->Release();
	}
}
