#include "single_ll.hpp"
#include "core/logger.hpp"
#include "memory/hmemory.hpp"

i8 create_slist(slist* list, u64 size)
{
	list->data_size = size;
	list->head = nullptr;
	list->tail = nullptr;

	return H_OK;
}

i8 push_back(slist* list, void* obj)
{
	if (nullptr == list->head && nullptr == list->tail)
	{
		list->head = (slist_t*)hmemory_alloc(list->data_size, MEM_TAG_SLIST);
		list->tail = list->head;
	}
	else
	{
		list->tail->next = (slist_t*)hmemory_alloc(list->data_size, MEM_TAG_SLIST);
		list->tail = list->tail->next;
	}

	hmemory_copy(list->tail->data, obj, list->data_size);
	
	return H_OK;
}

i8 pop_front(slist* list)
{
	slist_t* temp = list->head;
	if (list->head == list->tail)
	{
		list->head = nullptr;
		list->tail = nullptr;
	}
	else
	{
		list->head = list->head->next;
	}

	hmemory_free(temp->data, MEM_TAG_UNKNOWN);
	hmemory_free(temp, MEM_TAG_SLIST);

	return H_OK;
}

slist_t* get_front(slist* list)
{
	return list->head;
}

slist_t* get_last(slist* list)
{
	return list->tail;
}

void destroy_slist(slist* list)
{
	while (nullptr != list->head)
	{
		pop_front(list);
	}
}


