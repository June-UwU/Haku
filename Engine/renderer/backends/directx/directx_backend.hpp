/*****************************************************************//**
 * \file   directx_backend.hpp
 * \brief  directx_backend header that is used for directx backend internal purposes, by other backend modules, this should be unknown to renderer front end
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include "directx_types.INL"

/**
 * routine to request direct commandlist to put work into it, this .
 * 
 * \param type type of list to request
 * \return pointer to directx_list or nullptr
 */
directx_commandlist* request_commandlist(queue_type type);

directx_device* get_device();


