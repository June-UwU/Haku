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
 * \param list pointer to commandlist that will be seeded and placed as required
 * \param type type of list to request
 * \return H_OK on sucess
 */
i8 request_commandlist(directx_commandlist* list,queue_type type);




