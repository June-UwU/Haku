/*****************************************************************//**
 * \file   pipeline_state.hpp
 * \brief  pipeline configuration handling pipeline state
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/


#include "defines.hpp"
#include "directx_types.INL"

/**
 * directx pipeline state creation routine.
 * 
 * \param module pointer to create pipeline state with the pipeline state
 */
directx_pipeline* create_pipeline_state(directx_context* context, directx_shader_module* module);

/**
 * destroy pipeline state that was previously created pipeline state.
 *
 * \param pipeline pointer to the created pipeline state
 */
void destroy_pipeline_state(directx_pipeline* pipeline);

/**
 * routine to bind the current pipeline state to the frame.
 * 
 * \param commandlist pointer to the recording commandlist
 * \param pipeline pointer to the pipeline to bind
 */
void bind_pipeline_state(directx_commandlist* commandlist, directx_pipeline* pipeline);




