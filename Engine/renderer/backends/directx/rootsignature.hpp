/*****************************************************************/ /**
* \file   rootsignature.hpp
* \brief  directx root signature for a pipeline state
*
* \author June
* \date   September 2022
*********************************************************************/

#include "defines.hpp"
#include "directx_types.INL"

/** macro to create a root signature , parameter and sampler array and assign it to the pipeline*/
#define CREATE_PSO_ROOT_SIGNATURE(pso)                                                                                 \
	pso->signature	= (directx_root_signature*)hmemory_alloc(sizeof(directx_root_signature), MEM_TAG_RENDERER);        \
	darray param_array	= HAKU_CREATE_DARRAY(D3D12_ROOT_PARAMETER1);                                                   \
	darray sample_array = HAKU_CREATE_DARRAY(D3D12_ROOT_PARAMETER1);                                                   \
	pso->signature->parameter_array = param_array;                                                                     \
	pso->signature->sampler_array	= sample_array;

/**
 * routine to create root signature that is bindable to the pipeline.
 *
 * \param signature pointer to place the signature to
 * \return H_OK on sucess and H_FAIL on failure
 */
i8 create_root_signature(directx_device* device, directx_root_signature* signature);

/**
 * routine to destroy a created root signature.
 *
 * \param signature pointer to the signature to destroy
 */
void destroy_root_signature(directx_root_signature* signature);

/**
 * routine to add a root constant to a root signature.
 *
 * \param signature  pointer to root signature
 * \param num_of_values number of values to parameter
 * \param register_space register parameter
 * \return H_OK on sucess and H_FAIL on fail
 */
i8 add_root_constant(directx_root_signature* signature, const u64 num_of_values, const u64 register_space = 0);

/**
 * routine to add a srv root descriptor to the root signature.
 *
 * \param signature  pointer to root signature
 * \param register space
 * \param flags flags of the parameter
 * \return H_OK on sucess and H_FAIL on fail
 */
i8 add_root_descriptor_srv(
	directx_root_signature*		signature,
	const u64					register_space = 0,
	D3D12_ROOT_DESCRIPTOR_FLAGS flags		   = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);

/**
 * routine to add a uav root descriptor to the root signature.
 *
 * \param signature  pointer to root signature
 * \param register space
 * \param flags flags of the parameter
 * \return H_OK on sucess and H_FAIL on fail
 */
i8 add_root_descriptor_uav(
	directx_root_signature*		signature,
	const u64					register_space = 0,
	D3D12_ROOT_DESCRIPTOR_FLAGS flags		   = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);

/**
 * routine to add a cbv root descriptor to the root signature.
 *
 * \param signature  pointer to root signature
 * \param register space
 * \param flags flags of the parameter
 * \return H_OK on sucess and H_FAIL on fail
 */
i8 add_root_descriptor_cbv(
	directx_root_signature*		signature,
	const u64					register_space = 0,
	D3D12_ROOT_DESCRIPTOR_FLAGS flags		   = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);

/**
 * routine to add a srv root descriptor to the root signature.
 *
 * \param signature  pointer to root signature
 * \param table_array pointer to the dynamic array that contains table
 * \return H_OK on sucess and H_FAIL on fail
 */
i8 add_descriptor_table(directx_root_signature* signature, darray table_array);

/**
 * routine to add a descriptor to darray.
 *
 * \param signature pointer to signature
 * \param table pointer to the table
 * \param type type of parameter to add
 * \param number_of_descriptor number of descriptor parameter
 * \param register_space register space
 * \param flags flags parameter
 * \return H_OK on sucess and H_FAIL on fail
 */
i8 add_table_parameter(
	directx_root_signature*		 signature,
	darray*						 table,
	descriptor_type				 type,
	const u64					 number_of_descriptor,
	const u64					 register_space = 0,
	D3D12_DESCRIPTOR_RANGE_FLAGS flags			= D3D12_DESCRIPTOR_RANGE_FLAG_NONE);

i8 bind_root_signature(directx_cc* commandlist,directx_root_signature* signature);
