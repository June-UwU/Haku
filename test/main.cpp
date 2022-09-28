#include "test.hpp"
#include <gtest/gtest.h>
#include "platform/platform.hpp"
#include "memory/linear_allocator.hpp"
#include "generics/single_ll.hpp"
#include "generics/hash_set.hpp"
#include "memory/hmemory.hpp"
#include "generics/queue.hpp"

TEST(hash_set, hash_set_functionailty_tests)
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

TEST(single_ll, linked_list_test)
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

TEST(linear_allocator_test, linear_allocator_test)
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

TEST(queue_t_test, haku_queue_test)
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

int main(int argc, char** argv) 
{
	prepare_subsystem_for_test();

	::testing::InitGoogleTest(&argc, argv);
	int  ret_val = RUN_ALL_TESTS();
	
	shutdown_test_subsystem();
	return ret_val;
}