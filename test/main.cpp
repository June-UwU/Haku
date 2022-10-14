/*****************************************************************//**
 * \file   main.cpp
 * \brief  haku unit tests
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "test.hpp"
#include <gtest/gtest.h>
#include "platform/platform.hpp"
#include "memory/linear_allocator.hpp"
#include "generics/single_ll.hpp"
#include "generics/hash_set.hpp"
#include "memory/hmemory.hpp"
#include "generics/queue.hpp"
#include "generics/hash_map.hpp"

TEST(generics_tests, hash_set_functionailty_tests)
{
    hash_set_t  table;
    HAKU_CREATE_INT_HASH_SET(&table, u64);
    bool passed = true;

    u64 test_range = 512;

    // no collision test
    for (u64 i = 0; i < test_range; i++)
    {
        push_back(&table, &i, &i);
    }

    for (u64 i = 0; i < test_range; i++)
    {
        hash_set_entry_t* ret_entry = find(&table, &i);
        if (nullptr != ret_entry)
        {
            u64* ret_ptr = (u64*)front(ret_entry->data)->data;
            EXPECT_EQ(*ret_ptr, i);
        }

    }

    //collision test

    for (u64 i = 0; i < test_range; i++)
    {
        push_back(&table, &i, &i);
    }

    for (u64 i = 0; i < test_range; i++)
    {
        hash_set_entry_t* ret_entry = find(&table, &i);
        queue_t* list = ret_entry->data;
        for_queue_t(list, entry->next != NULL_PTR)
        {
            u64* ret_ptr = (u64*)entry->data;
            EXPECT_EQ(*ret_ptr, i);
        }
    }


    for (u64 i = 0; i < test_range; i++)
    {
        if (0 == i % 2)
        {
            remove_entry(&table, &i);
        }
    }

    for (u64 i = 0; i < test_range; i++)
    {
        if (0 == i % 2)
        {
            hash_set_entry_t* ret_entry = find(&table, &i);
            EXPECT_TRUE(ret_entry == nullptr);
        }
    }

    destroy_hash_table(&table);
}

TEST(generics_tests, linked_list_test)
{
	slist* list = (slist*)malloc(sizeof(slist));
	HAKU_CREATE_SLIST(list, u64);

	for (u64 i = 0; i < 256; i++)
	{
		push_back(list, &i);
	}

	slist_t* current = list->head;


	u64 i = 0;
	while (nullptr != current)
	{
		u64* data = (u64*)current->data;
        EXPECT_EQ(*data, i);
		i++;
		current = current->next;
	}
	destroy_slist(list);
}

TEST(memory_tests, linear_allocator_test)
{
	linear_allocator* allocator = create_linear_allocator(512);

	EXPECT_EQ(allocator->offset, 0);

	void* ret_ptr = linear_allocate(allocator, allocator->size_in_bytes - 1 );

	EXPECT_TRUE(ret_ptr);
	
	ret_ptr       = linear_allocate(allocator, allocator->size_in_bytes);

	EXPECT_TRUE(nullptr == ret_ptr);

	linear_allocator_reset(allocator);

	EXPECT_TRUE(allocator->offset == 0);

}

TEST(generics_tests, haku_queue_test)
{
	queue_t queue;
	u64 test_range = 512;

	i8 ret_val = HAKU_CREATE_QUEUE(&queue, u64);

	for (u64 i = 0; i < test_range; i++)
	{
		enqueue(&queue, &i);
	}


	int check_val = 0;
	for_queue_t(&queue, entry != nullptr)
	{
		u64* data_ptr = (u64*)entry->data;
		EXPECT_EQ(check_val, *data_ptr);
		check_val++;
	}

	queue_entry_t* entry = front(&queue);
	u64* data_ptr = (u64*)entry->data;
	EXPECT_EQ(0, *data_ptr);

	dequeue(&queue);

	entry = front(&queue);
	data_ptr = (u64*)entry->data;
	EXPECT_EQ(1, *data_ptr);

	reset(&queue);
	EXPECT_EQ(NULL_PTR, queue.front);
	EXPECT_EQ(NULL_PTR, queue.rear);

	destroy_queue(&queue);
}

TEST(generics_tests, hash_map_test)
{
	hash_map_t map;
	u32 test_range = 20;

	create_hash_map(&map, sizeof(u64), HASH_MAP_INT_HASH);

	hash_map_entry_t* ret_entry = find(&map, 69);
	EXPECT_EQ(nullptr,ret_entry);

	for (u64 i = 0; i < test_range; i++)
	{
		push_back(&map, i, &i);
	}
	
	for (u64 i = 0; i < test_range; i++)
	{
		hash_map_entry_t* ret_entry = find(&map, i);
		if (ret_entry)
		{
			u64* ret_val = (u64*)ret_entry->data;
			EXPECT_EQ(i, *ret_val);
		}
		EXPECT_TRUE(nullptr != ret_entry);

	}

	for (u64 i = 0; i < test_range; i++)
	{
		if (0 == (i % 2))
		{
			remove_entry(&map, i);
		}
	}

	for (u64 i = 0; i < test_range; i++)
	{
		hash_map_entry_t* ret_entry = find(&map, i);
		if (0 == (i % 2))
		{
			EXPECT_EQ(nullptr, ret_entry);
		}
		else
		{
			u64* ret_val = (u64*)ret_entry->data;
			EXPECT_EQ(i, *ret_val);
		}
	}

	destroy_hash_map(&map);
}


TEST(generic_test, bound_test)
{
	hash_map_t hash_map;
	hash_set_t hash_set;

	create_hast_table(&hash_set, sizeof(u64), INT_HASH);
	create_hash_map(&hash_map, sizeof(u64), HASH_MAP_INT_HASH);

	for (u64 i = 10; i < 64; i++)
	{
		push_back(&hash_map, i, &i);
		push_back(&hash_set, &i, &i);
	}

	for (u64 i = 11; i < 64; i++)
	{
		hash_set_entry_t* ret_set_l = lower_bound(&hash_set, i);
		EXPECT_EQ(ret_set_l->key, i);

		hash_map_entry_t* ret_map_l = lower_bound(&hash_map, i);
		EXPECT_EQ(ret_map_l->key, i);

		hash_set_entry_t* ret_set_u = upper_bound(&hash_set, i);
		EXPECT_LT(ret_set_u->key, i);
		EXPECT_EQ(ret_set_u->key, i - 1);

		hash_map_entry_t* ret_map_u = upper_bound(&hash_map, i);
		EXPECT_LT(ret_map_u->key, i);
		EXPECT_EQ(ret_map_u->key, i - 1);
	}
	destroy_hash_map(&hash_map);
	destroy_hash_table(&hash_set);
}

int main(int argc, char** argv) 
{
	prepare_subsystem_for_test();

	::testing::InitGoogleTest(&argc, argv);
	int  ret_val = RUN_ALL_TESTS();
	
	shutdown_test_subsystem();
	return ret_val;
}