/*****************************************************************//**
 * \file   single_ll.hpp
 * \brief  haku internally defined singly linked list
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/**
 * helper macro to create a slist.
 * 
 * \param plist pointer to list
 * \param obj an object or struct to be made for this list
 * \return H_OK or H_FAIL
 */
#define HAKU_CREATE_SLIST(plist,obj) create_slist(plist,sizeof(obj))

 /** a single entry in the list */
typedef struct slist_t
{
	/** data pointer  */
	void* data;

	/** next element, nullptr if last entry */
	slist_t* next;
}slist_t;

/** singly linked list head that holds meta data that is used  */
typedef struct slist
{
	/** size of the data in slist_t in bytes */
	u64 data_size;

	/** last entry  */
	slist_t* tail;

	/** first entry */
	slist_t* head;
}slist;

/**
 * routine to create a singly linked list.
 * 
 * \param list slist that will represent the list
 * \param size size of the element that is held in the list
 * \return H_OK on sucess or H_FAIL on failure
 */
HAPI i8 create_slist(slist* list,u64 size);

/**
 * routine to push a element into a singly linked list.
 *
 * \param list slist that will represent the list
 * \param obj element that is held in the list
 * \return H_OK on sucess or H_FAIL on failure
 */
HAPI i8 push_back(slist* list, void* obj);

/**
 * routine to push back a list to the back of another list.
 * 
 * \param list list that will be pushed on to
 * \param push_list list that will pushed 
 */
HAPI i8 push_back(slist* list, slist* push_list);

/**
 * routine to pop from a singly linked list.
 *
 * \param list slist that will represent the list
 * \return H_OK on sucess or H_FAIL on failure
 */
HAPI i8 pop_front(slist* list);

/**
 * routine to get first from a singly linked list.
 *
 * \param list slist that will represent the list
 * \return H_OK on sucess or H_FAIL on failure
 */
HAPI slist_t* get_front(slist* list);

/**
 * routine to get a last element of a list.
 *
 * \param list slist that will represent the list
 * \return H_OK on sucess or H_FAIL on failure
 */
HAPI slist_t* get_last(slist* list);

/**
 * routine to clean up a list.
 *
 * \param list slist that will represent the list
 */
HAPI void destroy_slist(slist* list);

/**
 * routine to reset the list.
 * 
 * \param list pointer to the list to be reset
 */
HAPI void reset(slist* list);

/** singly linked list test routine */
void test_slist(void);
