#pragma once
/*****************************************************************//**
 * \file   shader.hpp
 * \brief  shader compilation and creation system
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "defines.hpp"
#include "directx_types.INL"

// @breif	entry point for every shader is fixed
constexpr const char* SHADER_MAIN = "main";

/** shader profile under which it will be compiled against  */
constexpr const char* SHADER_PROFILE_11_0[HK_SHADER_MAX]{
	"vs_5_0", // vertex shader stage target enumerations
	"hs_5_0", // hull shader stage target enumerations
	"ds_5_0", // domain shader stage target enumerations
	"gs_5_0", // geometry shader stage target enumerations
	"ps_5_0", // pixel shader stage target enumerations
	"cs_5_0"  // compute shader stage target enumerations
};

/** shader profile for 10.0  support */
constexpr const char* SHADER_PROFILE_10_0[HK_SHADER_MAX]{
	"vs_4_0", // vertex shader stage enumerations
	"",		  // hull shader doesn't exist here
	"",		  // domain shader doesn't exist here
	"gs_4_0", // geometry shader stage enumerations
	"ps_4_0", // pixel shader stage enumerations
	"cs_4_0"  // compute shader stage enumerations
};

/**
 * function to compile shaders at runtime.
 * 
 * \param context pointer to directx_context 
 * \param path pointer to the file path
 * \param stage stage for the directx compile
 * \param module module to compile and place the shader into
 * \return H_OK on sucess else H_FAIL
 */
i8 compile_shader(const directx_context* context, const char* path, shader_stages stage,directx_shader_module* module);

/**
 * function to create code byte code blob from a compiled shader.
 *
 * \param context pointer to directx_context
 * \param path pointer to the file path
 * \param stage stage for the directx compile
 * \param module module to compile and place the shader into
 * \return H_OK on sucess else H_FAIL
 */
i8 create_shader_byte_code(const directx_context* context, const char* path, shader_stages stage, directx_shader_module* module);


