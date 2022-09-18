/*****************************************************************//**
 * \file   single_ll.cpp
 * \brief  singly linked list 
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "single_ll.hpp"
#include "core/logger.hpp"
#include "memory/hmemory.hpp"

#include <cstring>

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
		list->head = (slist_t*)hmemory_alloc(sizeof(slist_t),MEM_TAG_SLIST);
		list->tail = list->head;
	}
	else
	{
		list->tail->next = (slist_t*)hmemory_alloc(sizeof(slist_t),MEM_TAG_SLIST);
		list->tail = list->tail->next;
	}
	list->tail->data = hmemory_alloc(list->data_size, MEM_TAG_SLIST);
	hmemory_copy(list->tail->data, obj, list->data_size);
	list->tail->next = nullptr;


	return H_OK;
}

i8 push_back(slist* list, slist* push_list)
{

	if (nullptr == list->head && nullptr == list->tail)
	{
		list->head = push_list->head;
		list->tail = push_list->tail;
	}
	else
	{
		list->tail->next = push_list->head;
	}
	return H_OK;
}

i8 pop_front(slist* list)
{
	slist_t* temp = list->head;
	if (nullptr == temp)
	{
		return H_NOERR;
	}

	if (list->head == list->tail)
	{
		list->head = nullptr;
		list->tail = nullptr;
	}
	else
	{
		list->head = temp->next;
	}

	hmemory_free(temp->data,MEM_TAG_SLIST);
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
		slist_t* del_ptr = list->head;
		list->head = del_ptr->next;
		hmemory_free(del_ptr->data, MEM_TAG_SLIST);
		hmemory_free(del_ptr, MEM_TAG_SLIST);
	}
	list->head = nullptr;
	list->tail = nullptr;
}

void reset(slist* list)
{
	slist_t* old_head = list->head;
	list->head = nullptr;
	list->tail = nullptr;
	slist_t* del_ptr = old_head;
	while (nullptr != old_head->next)
	{
		old_head = old_head->next;
		free(del_ptr);
		del_ptr = old_head;
	}
	free(del_ptr);
}

void test_slist(void)
{
	slist* list = (slist*)malloc(sizeof(slist));
	HAKU_CREATE_SLIST(list, u64);

	for (u64 i = 0; i < 256; i++)
	{
		push_back(list, &i);
	}

	slist_t* current = list->head;
	

	u64 i = 0;
	while( nullptr != current)
	{
		u64* data = (u64*)current->data;
		if (*data != i)
		{
			HLEMER("logical error in slist");
		}
		i++;
		current = current->next;
	}
	HLINFO("slist test passed..!");
	destroy_slist(list);
}
