/*****************************************************************//**
 * \file   queue.cpp
 * \brief  haku queue implementations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "queue.hpp"
#include "memory/hmemory.hpp"
#include "core/logger.hpp"

/**
 * routine to expand the queue for additional entry.
 * 
 * \param queue pointer to the queue
 */
void expand_queue(queue_t* queue);

i8 create_queue(queue_t* queue, u64 element_size)
{
	queue->element_size = element_size;
	queue->current_entry_count = 0;
	queue->capacity = QUEUE_INIT_SIZE;
	queue->data_array = hmemory_alloc(element_size * queue->capacity, MEM_TAG_QUEUE);
	queue->entry_array = (queue_entry_t*)hmemory_alloc(queue->capacity * sizeof(queue_entry_t), MEM_TAG_QUEUE);
	queue->free_index = (bool*)hmemory_alloc_zeroed(queue->capacity * sizeof(bool), MEM_TAG_QUEUE);
	queue->front = NULL_PTR;
	queue->rear = NULL_PTR;


	return H_OK;
}

void destroy_queue(queue_t* queue)
{
	hmemory_free(queue->entry_array, MEM_TAG_QUEUE);
	hmemory_free(queue->free_index, MEM_TAG_QUEUE);
	hmemory_free(queue->data_array, MEM_TAG_QUEUE);
}

void enqueue(queue_t* queue, void* obj)
{
	if ((queue->capacity - queue->current_entry_count) < 8u)
	{
		expand_queue(queue);
	}

	u64 index = 0;
	if (NULL_PTR == queue->front && NULL_PTR == queue->rear)
	{
		queue->front = index;
		queue->rear = index;
		queue->entry_array[index].next = NULL_PTR;
	}
	else
	{
		while (true == queue->free_index[index] && index <  queue->capacity)
		{
			index++;
		}
		queue->entry_array[queue->rear].next = index;
		queue->rear = index;
	}
	queue->free_index[index] = true;


	u64 byte_offset = queue->element_size * index;
	i8* entry_dest = (i8*)queue->data_array;
	entry_dest = entry_dest + byte_offset;
	
	queue->entry_array[index].next = NULL_PTR;
	queue->entry_array[index].data = entry_dest;

	hmemory_copy(entry_dest, obj, queue->element_size);
	queue->current_entry_count++;
}

void enqueue(queue_t* dest_queue, queue_t* src_queue)
{
	HLCRIT("queue on queue push ");
	for_queue_t(src_queue, entry->next != NULL_PTR)
	{
		enqueue(dest_queue, entry->data);
	}
}

void* dequeue(queue_t* queue)
{
	i64 stale_front = queue->front;
	if (NULL_PTR == stale_front)
	{
		return nullptr;
	}

	if (queue->front == queue->rear)
	{
		queue->front = NULL_PTR;
		queue->rear = NULL_PTR;
	}
	else
	{
		queue->front = queue->entry_array[queue->front].next;
	}
	queue->free_index[stale_front] = false;
	queue->current_entry_count--;

	return queue->entry_array[stale_front].data;
}

queue_entry_t* next(queue_t* queue, queue_entry_t* entry)
{
	u64 next = entry->next;

	if (NULL_PTR == next)
	{
		return nullptr;
	}
	entry = queue->entry_array + next;
	return entry;
}

void reset(queue_t* queue)
{
	queue->front = NULL_PTR;
	queue->rear = NULL_PTR;
	queue->current_entry_count = 0;
	hmemory_set(queue->free_index, false, queue->capacity * sizeof(bool));
}

queue_entry_t* front(queue_t* queue)
{
	u64 front = queue->front;

	if (NULL_PTR == front)
	{
		return nullptr;
	}
	return &queue->entry_array[front];
}

queue_entry_t* back(queue_t* queue)
{
	u64 back = queue->rear;

	if (NULL_PTR == back)
	{
		return nullptr;
	}
	return &queue->entry_array[back];
}


void expand_queue(queue_t* queue)
{
	queue_entry_t* stale_entry = queue->entry_array;
	void* stale_data_array = queue->data_array;
	bool* stale_free_index = queue->free_index;
	u64 stale_capacity = queue->capacity;
	queue->capacity = 2 * queue->capacity;

	queue->data_array = hmemory_alloc(queue->element_size * queue->capacity, MEM_TAG_QUEUE);
	queue->entry_array = (queue_entry_t*)hmemory_alloc(queue->capacity * sizeof(queue_entry_t), MEM_TAG_QUEUE);
	queue->free_index = (bool*)hmemory_alloc_zeroed(queue->capacity * sizeof(bool), MEM_TAG_QUEUE);

	hmemory_copy(queue->data_array, stale_data_array, stale_capacity * queue->element_size);
	hmemory_copy(queue->free_index, stale_free_index, stale_capacity * sizeof(bool));
	hmemory_copy(queue->entry_array, stale_entry, stale_capacity * sizeof(queue_entry_t));
	
	u64 index = queue->front;
	u64* data_ptr = (u64*)queue->data_array;
	for_queue_t(queue, entry->next != NULL_PTR)
	{
		entry->data = data_ptr + index;
		index = entry->next;
	}

	hmemory_free(stale_entry, MEM_TAG_QUEUE);
	hmemory_free(stale_data_array, MEM_TAG_QUEUE);
	hmemory_free(stale_free_index, MEM_TAG_QUEUE);

	HLWARN("Queue expanded %p\n\t capacity : %d", queue, queue->capacity);
}


