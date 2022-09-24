/*****************************************************************//**
 * \file   buffer.hpp
 * \brief   directx_buffer that is used
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/

/* FIX ME : implement a good garabage collector for the upload heap and live buffer https://stackoverflow.com/questions/6866531/how-to-implement-a-garbage-collector*/

#include "directx_types.INL"

/**
 * for proper uploads the intermediate buffer needs to be gpu resident till the copy is over,this routine initiates the nessary
 * data structures for that.
 * 
 * \return H_OK on sucess
 */
i8 initiate_upload_structure(void);

/**
 * for proper uploads the intermediate buffer needs to be gpu resident till the copy is over,this routine cleans up the nessary
 * data structures for that.
 * 
 * \param fence_val shutdown fence value
 */
void shutdown_upload_structure(u64 fence_val);

// TODO : make possible transitions for the buffer and set the states
/**
 * routine to create directx 12 resources that are bound to the pipeline for different purposes.
 * 
 * \param context pointer to the directx renderer context
 * \param buffer pointer to the buffer to fill out
 * \param data pointer to the data to be filled in the buffer
 * \param size size of the buffer in bytes in the data
 * \param type resource type
 * \return H_OK on sucess and H_FAIL on failure
 */
i8 create_buffer(const directx_context* context,directx_buffer* buffer, i8* data, u64 size,resource_type type);

void release_buffer(directx_buffer* buffer);

i8 resize_buffer();

i8 copy_buffer();

// TODO : does state assioate with the buffer and do we need a transition function for it ? resource creation need to be checked
/**
 * routine to remove stale upload buffer synchronized to the gpu.
 * 
 * \param fence_val completed fence val
 * \return number of resources freed
 */
i64 remove_stale_upload_buffer(u64 fence_val);




