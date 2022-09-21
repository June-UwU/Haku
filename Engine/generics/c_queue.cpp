/*****************************************************************//**
 * \file   c_queue.cpp
 * \brief  haku circular queue implementations
 * 
 * \see https://www.geeksforgeeks.org/circular-queue-set-1-introduction-array-implementation/
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "c_queue.hpp"
#include "core\logger.hpp"
#include "memory\hmemory.hpp"

// TODO : test

// made slight modifications nessary for the purge part

c_queue* create_c_queue(u64 size, u64 element_size, bool purge)
{
	c_queue* ret_ptr = (c_queue*)hmemory_alloc(sizeof(struct c_queue), MEM_TAG_CQUEUE);

	ret_ptr->size		  = 0;
	ret_ptr->element_size = element_size;
	ret_ptr->capacity	  = size;
	ret_ptr->front		  = -1;
	ret_ptr->rear		  = -1;
	ret_ptr->purge		  = purge;
	ret_ptr->cq_ptr		  = hmemory_alloc(size * element_size, MEM_TAG_CQUEUE);

	return ret_ptr;
}

void destroy_c_queue(c_queue* q_ptr)
{
	hmemory_free(q_ptr->cq_ptr, MEM_TAG_CQUEUE);
	hmemory_free(q_ptr, MEM_TAG_CQUEUE);
}

void* peek(c_queue* q_ptr)
{
	i8* byte_ptr = (i8*)q_ptr->cq_ptr;

	byte_ptr = byte_ptr + (q_ptr->front * q_ptr->element_size);

	return byte_ptr;
}

i8 enqueue(c_queue* q_ptr, void* obj)
{
	i8* byte_ptr = (i8*)q_ptr->cq_ptr;
	if ((q_ptr->front == 0 && (q_ptr->rear == q_ptr->capacity - 1)) ||
		(q_ptr->rear == ((q_ptr->front - 1) % q_ptr->capacity)))
	{
		if (true == q_ptr->purge)
		{
			HLWARN("Purge queue rewrite : %x", q_ptr);
			q_ptr->rear = q_ptr->front;
			if (q_ptr->front == (q_ptr->capacity - 1))
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
			HLWARN("Queue full : %x", q_ptr);
			return H_ERR;
		}
	}
	else if (q_ptr->front == -1)
	{
		q_ptr->front = q_ptr->rear = 0;
		q_ptr->size += 1;
		goto queue_copy;
	}
	else if ((q_ptr->rear == q_ptr->capacity - 1) && (q_ptr->front != 0))
	{
		q_ptr->rear = 0;
		q_ptr->size += 1;
		goto queue_copy;
	}
	else
	{
		q_ptr->rear = q_ptr->rear + 1;
		q_ptr->size += 1;
		goto queue_copy;
	}

queue_copy:
	byte_ptr = byte_ptr + (q_ptr->rear * q_ptr->element_size);
	hmemory_copy(byte_ptr, obj, q_ptr->element_size);
	return H_OK;
}

void* dequeue(c_queue* q_ptr)
{
	i8* ret_ptr = (i8*)q_ptr->cq_ptr;

	if (-1 == q_ptr->front)
	{
		HLWARN("Dequeue on empty queue : %x", q_ptr);
		return nullptr;
	}

	ret_ptr = ret_ptr + (q_ptr->front * q_ptr->element_size);

	q_ptr->size -= 1;
	if (q_ptr->front == q_ptr->rear)
	{
		q_ptr->front = q_ptr->rear = -1;
	}
	else if (q_ptr->front == (q_ptr->capacity - 1))
	{
		q_ptr->front = 0;
	}
	else
	{
		q_ptr->front++;
	}

	return ret_ptr;
}

u64 size(const c_queue* q_ptr)
{
	return q_ptr->size;
}

u64 capacity(const c_queue* q_ptr)
{
	return q_ptr->capacity;
}

void c_queue_test()
{
	const int size = 4;
	c_queue*  q	   = create_c_queue(size, sizeof(u64), false);
	c_queue*  pq   = create_c_queue(size, sizeof(u64), true);

	HLINFO("Enqueuing queues");
	for (u64 i = 0; i < 2 * size; i++)
	{
		enqueue(q, &i);
	}

	HLINFO("Enqueuing purge queues");
	for (u64 i = 0; i < 2 * size; i++)
	{
		enqueue(pq, &i);
	}

	u64* ret_ptr = NULL;
	HLINFO("Dequeuing queue");
	for (u64 i = 0; i <= size; i++)
	{
		ret_ptr = (u64*)dequeue(q);
		if (nullptr != ret_ptr)
		{
			HLINFO("%ld", *ret_ptr);
		}
	}

	HLINFO("Dequeuing purge queue");
	for (u64 i = 0; i <= size; i++)
	{
		ret_ptr = (u64*)dequeue(pq);
		if (nullptr != ret_ptr)
		{
			HLINFO("%ld", *ret_ptr);
		}
	}
}
