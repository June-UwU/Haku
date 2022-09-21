/*****************************************************************//**
 * \file   c_queue.hpp
 * \brief  haku circular queue
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/** Haku generic structure : circular queue */
typedef struct c_queue
{
	/** capacity of the queue */
	u64	  capacity;	

	/** index of front */
	i64	  front;		

	/** index of rear */
	i64	  rear;		

	/** size of the queue */
	u64	  size;			

	/** element size of the queue [size of queue in bytes  = element size * size] */
	u64	  element_size; 

	/** queue ptr */
	void* cq_ptr;

	/** push to the front of by adjusting the queue front */
	bool  purge;		
} c_queue;

/**
 * routine to initialize the queue.
 * 
 * \param size size of the queue
 * \param element_size element size of element
 * \param purge boolean purge size
 * \return pointer to the queue structure created
 */
HAPI c_queue* create_c_queue(u64 size, u64 element_size, bool purge);

/**
 * routine to shutdown a queue.
 * 
 * \param q_ptr pointer to the queue
 */
HAPI void destroy_c_queue(c_queue* q_ptr);

/**
 * routine to get a pointer to the front of the queue.
 * 
 * \param q_ptr pointer to the queue
 * \return pointer to the first element 
 */
HAPI void* peek(c_queue* q_ptr);

/**
 * routine to enqueue an element to the back of the queue.
 * 
 * \param q_ptr pointer to the queue object
 * \param obj opaque pointer to the object to be hmemory_copied
 * \return H_OK on sucess and H_FAIL on fail
 */
HAPI i8 enqueue(c_queue* q_ptr, void* obj);

/**
 * routine to dequeue an element from the front.
 * 
 * \param q_ptr pointer to the c_queue structure
 * \return pointer to the first element that was removed
 */
HAPI void* dequeue(c_queue* q_ptr);

/**
 * routine to get the current size of c_queue.
 * 
 * \param q_ptr pointer to c_queue
 * \return current size of the buffer
 */
HAPI u64 size(const c_queue* q_ptr);

/**
 * routine to get the capacit of c_queue.
 * 
 * \param q_ptr pointer to the c_queue
 * \return capacity of the c_queue
 */
HAPI u64 capacity(const c_queue* q_ptr);

void c_queue_test();


