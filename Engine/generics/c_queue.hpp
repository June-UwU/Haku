#include "defines.hpp"

// Haku generic structure : circular queue

typedef struct c_queue
{
	i64	front;			// index of front 
	i64 	rear;			// index of rear
	u64 	size;			// size of the queue [CAN'T BE CHANGED AFTER INITIALIZE]
	u64	element_size;		// element size of the queue [size of queue in bytes  = element size * size]
	void* 	cq_ptr;			// queue ptr
	bool 	purge;		// push to the front of by adjusting the queue front
}c_queue;


// @breif 	routine to initialize the queue
// @param 	: size of the queue
// @param	: element size of element
// @param	: boolean purge size
// @return 	: pointer to the queue structure created
HAPI c_queue* c_queue_initialize(u64 size, u64 element_size, bool purge);

// @breif	routine to shutdown a queue
// @param	: pointer to the queue
HAPI void c_queue_shutdown(c_queue* q_ptr);

// @breif	routine to get a pointer to the front of the queue
// @param	: pointer to the queue
// @return	: pointer to the first element
HAPI void* peek(c_queue* q_ptr);

// @breif 	routine to enqueue an element to the back of the queue
// @param	: pointer to the queue object
// @param 	: opaque pointer to the object to be hmemory_copied
// @return 	: H_OK on sucess and H_FAIL on fail
HAPI i8 enqueue(c_queue* q_ptr, void* obj);

// @breif 	routine to dequeue an element from the front
// @param	: pointer to the c_queue structure
// @return 	: pointer to the first element that was removed
HAPI void* dequeue(c_queue* q_ptr);
