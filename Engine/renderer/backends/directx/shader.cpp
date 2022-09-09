/*****************************************************************//**
 * \file   shader.cpp
 * \brief  shader compilation and implementation file
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma comment(lib , "D3DCompiler.lib")

#include "shader.hpp"
#include "core/logger.hpp"


i8 compile_shader(const directx_context* context, const char* path, shader_stages stage, directx_shader_module* module)
{
	wchar_t* wide_path = nullptr;
	mbstowcs(wide_path, path, strlen(path));

	const char* target_ptr = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* error_blob = nullptr;

	if (nullptr == wide_path)
	{
		HLCRIT("path is none");
		return H_FAIL;
	}

	HRESULT api_ret_val = D3DCompileFromFile(wide_path,NULL,D3D_COMPILE_STANDARD_FILE_INCLUDE,SHADER_MAIN,
		SHADER_PROFILE_11_0[stage],flags,0,&module->compiled_shaders[stage],&error_blob
	);

	if (S_OK != api_ret_val)
	{
		HLCRIT("shader compilation failure : %s", (char*)error_blob->GetBufferPointer());
		return H_FAIL;
	}

	HLINFO("shader compilation for path : %s ", path);
	
	return H_OK;
}

i8 create_shader_byte_code(const directx_context* context, const char* path, shader_stages stage, directx_shader_module* module)
{
	wchar_t* wide_path = nullptr;
	mbstowcs(wide_path, path, strlen(path));

	if (nullptr == wide_path)
	{
		HLCRIT("shader byte code creation failed : %s ", path);
		return H_FAIL;
	}
	HRESULT api_ret_val = D3DReadFileToBlob(wide_path, &module->compiled_shaders[stage]);
	if (S_OK != api_ret_val)
	{
		HLCRIT("shader byte code creation failed : %s ", path);
		return H_FAIL;
	}

	return H_OK;
}
