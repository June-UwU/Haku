/*****************************************************************//**
 * \file   shader_compiler.cpp
 * \brief  shader compilation and implementation file
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma comment(lib , "D3DCompiler.lib")

#include "shader_compiler.hpp"
#include "core/logger.hpp"
#include "memory/hmemory.hpp"


ID3D10Blob* compile_shader(const char* path, shader_stages stage)
{
	wchar_t* wide_path = nullptr;
	mbstowcs(wide_path, path, strlen(path));

	const char* target_ptr = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* error_blob = nullptr;
	ID3DBlob* rp_blob = nullptr;
	if (nullptr == wide_path)
	{
		HLCRIT("path is none");
		return nullptr;
	}

	HRESULT api_ret_val = D3DCompileFromFile(wide_path,NULL,D3D_COMPILE_STANDARD_FILE_INCLUDE,SHADER_MAIN,
		SHADER_PROFILE_11_0[stage],flags,0,&rp_blob,&error_blob
	);

	if (S_OK != api_ret_val)
	{
		HLCRIT("shader compilation failure : %s", (char*)error_blob->GetBufferPointer());
		return nullptr;
	}

	HLINFO("shader compilation for path : %s ", path);
	
	return rp_blob;
}

ID3DBlob* create_shader_byte_code(const wchar_t* path, shader_stages stage)
{
	ID3DBlob* blob;
	HRESULT api_ret_val = D3DReadFileToBlob(path, &blob);
	if (S_OK != api_ret_val)
	{
		HLCRIT("shader byte code creation failed : %s ", path);
		return nullptr;
	}

	return blob;
}
