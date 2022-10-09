/*****************************************************************//**
 * \file   queue.hpp
 * \brief  haku queue
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/**
 * the queue size will be internally handled and this will behave like a queue but is implemented like a dynamic array.
 */

/**
 * macro to call the routine to create a queue that can hold obj.
 * 
 * \param queue_ptr pointer to the queue
 * \param obj object or type of the data
 */
#define HAKU_CREATE_QUEUE(queue_ptr,obj) create_queue(queue_ptr,sizeof(obj))

 /**
  * macro for doing a for loop,use entry for queue_entry_t*.
  *
  * \param initialize initialization statement
  * \param condition condition for the for loop
  */
#define for_queue_t(queue,condition) for(queue_entry_t* entry = front(queue);condition;entry = next(queue,entry))

/**
 * macro for doing a for each loop in queue.
 * 
 * \param queue pointer to a queue_t 
 */
#define for_each_queue_t(queue) for(queue_entry_t* entry = front(queue); entry != nullptr; entry = next(queue,entry))

/** symbol to signify that the entry pointer is not existent */
constexpr const i64 NULL_PTR = -1;

/** initial queue size */
constexpr const u64 QUEUE_INIT_SIZE = 32u;

/** structure holds a single queue entry */
typedef struct queue_entry_t
{
	/** the pointer that holds the data */
	void* data;

	/** pointer to the next entry */
	i64 next;
}queue_entry_t;

/** structure that represents the a queue */
typedef struct queue_t
{
	/** number of entries in the queue */
	u64 current_entry_count;

	/** total entry capacity of the queue */
	u64 capacity;

	/** the element size of the queue */
	u64 element_size;

	/** index to the front of the queue */
	i64 front;

	/** index to the back */
	i64 rear;

	/** array of memory to allocate entries from */
	queue_entry_t* entry_array;

	/** data array that is preallocated for the queue */
	void* data_array;

	/** free indexes for the array */
	bool* free_index;
}queue_t;


/**
 * routine to initialize queue.
 * 
 * \param queue  pointer to queue to be initialized
 * \param element_size size of the elements in the queue
 * \return H_OK on sucess
 */
HAPI i8 create_queue(queue_t* queue, u64 element_size);

/**
 * routine to destroy queue.
 * 
 * \param queue pointer to the queue
 */
HAPI void destroy_queue(queue_t* queue);

/**
 * routine to enqueue obj to the queue.
 * 
 * 
 * \param queue pointer to the queue to enqueue to
 * \param obj pointer to the object to memcopy
 */
HAPI void enqueue(queue_t* queue, void* obj);

/**
 * routine to push the src_queue elements to the dest_queue.
 * 
 * \param dest_queue pointer to the destination queue
 * \param src_queue pointer to the destination queue
 */
HAPI void enqueue(queue_t* dest_queue, queue_t* src_queue);
/**
 * routine to dequeue the first element in queue.
 * 
 * \param queue pointer to queue to dequeue from
 * \return pointer to the data[THE DATA IS VOLATILE AND IS ESSENTIALLY NOT SAFE]
 */
HAPI void* dequeue(queue_t* queue);

/**
 * routine to get the next queue_entry.
 * 
 * \param queue pointer to the queue
 * \param entry current entry
 * \return next entry
 */
HAPI queue_entry_t* next(queue_t* queue ,queue_entry_t* entry);

/**
 * routine to clear queue.
 * 
 * \param queue pointer to queue to free
 */
HAPI void reset(queue_t* queue);

/**
 * routine to get the first element  of the queue.
 * 
 * \param queue pointer to the queue
 * \return the pointer to the first element or nullptr
 */
HAPI queue_entry_t* front(queue_t* queue);

/**
 * routine to get the back of the queue.
 * 
 * \param queue pointer to the queue
 * \return the pointer to the last element or nullptr
 */
HAPI queue_entry_t* back(queue_t* queue);





