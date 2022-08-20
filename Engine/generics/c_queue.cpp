#include "c_queue.hpp"
#include "core\hmemory.hpp"
#include "core\logger.hpp"

// TODO : test

// THEROY : https://www.geeksforgeeks.org/circular-queue-set-1-introduction-array-implementation/
// made slight modifications nessary for the purge part

c_queue* c_queue_initialize(u64 size, u64 element_size, bool purge)
{
	c_queue* ret_ptr 	= (c_queue*)hmemory_alloc(sizeof(struct c_queue),MEM_TAG_CQUEUE);
	
	ret_ptr->element_size	= element_size;
	ret_ptr->size		= size;
	ret_ptr->front		= -1;
	ret_ptr->rear		= -1;
	ret_ptr->purge		= purge;
	ret_ptr->cq_ptr		= hmemory_alloc(size * element_size,MEM_TAG_CQUEUE);

	return ret_ptr;
}

void c_queue_shutdown(c_queue* q_ptr)
{
	hmemory_free(q_ptr->cq_ptr,MEM_TAG_CQUEUE);
	hmemory_free(q_ptr,MEM_TAG_CQUEUE);
}

void* peek(c_queue* q_ptr)
{
	i8* byte_ptr 	= (i8*)q_ptr->cq_ptr;

	byte_ptr 	= byte_ptr + (q_ptr->front * q_ptr->element_size);

	return byte_ptr;
}

i8 enqueue(c_queue* q_ptr, void* obj)
{
	i8* byte_ptr			= (i8*)q_ptr->cq_ptr;
	if((q_ptr->front ==0 && q_ptr->size-1) || (q_ptr->rear == (q_ptr->front - 1)%(q_ptr->size)))
	{
		if(true == q_ptr->purge)
		{
			q_ptr->rear	= q_ptr->front;
			if(q_ptr->front == (q_ptr->size -1))
			{
				q_ptr->front = 0;
			}
			else
			{
				q_ptr->front++;
			}
			goto queue_copy;
		}
		else
		{
			HLWARN("Queue full : %ld",q_ptr);
			return H_ERR;
		}
	}
	else if( q_ptr->front == -1 )
	{
		q_ptr->front 		= q_ptr->rear = 0;
		goto queue_copy;
	}
	else if( (q_ptr->rear == q_ptr->size - 1) && ( q_ptr->front != 0))
	{
		q_ptr->rear	 	= 0;
		goto queue_copy;
	}
	else
	{
		q_ptr->rear		= q_ptr->rear + 1;
		goto queue_copy;
	}

queue_copy:
	byte_ptr	= byte_ptr + (q_ptr->rear * q_ptr->element_size);
	hmemory_copy(byte_ptr,obj,q_ptr->element_size);
	return H_OK;
}

void* dequeue(c_queue* q_ptr)
{
	i8* ret_ptr	= (i8*)q_ptr->cq_ptr;
	
	if( -1 == q_ptr->front)
	{
		HLWARN("Dequeue on empty queue : %ld",q_ptr);
		return nullptr;
	}

	ret_ptr		= ret_ptr + (q_ptr->front * q_ptr->element_size);

	if( q_ptr->front == q_ptr->rear)
	{
		q_ptr->front = q_ptr->rear = -1;
	}
	else if ( q_ptr->front == (q_ptr->size-1))
	{
		q_ptr->front = 0;
	}
	else
	{
		q_ptr->front++;
	}

	return ret_ptr;
}
