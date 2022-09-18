#include "test.hpp"
#include <gtest/gtest.h>
#include "platform/platform.hpp"
#include "memory/linear_allocator.hpp"
#include "generics/hash_set.hpp"
#include "memory/hmemory.hpp"

TEST(hash_set, hash_set_functionailty_tests)
{
    hash_table_t table;
    HAKU_CREATE_INT_HASH_TABLE(&table, u64);

    // no collision test
    for (u64 i = 0; i < 512; i++)
    {
        i8* key_ptr = (i8*)&i;
        push_back(&table, key_ptr, &i);
    }

    for (u64 i = 0; i < 512; i++)
    {
        i8* key_ptr = (i8*)(&i);
        hash_table_entry* ret_entry = find(&table, key_ptr);
        if (nullptr != ret_entry->data)
        {
            u64* ret_ptr = (u64*)ret_entry->data->head->data;
			EXPECT_EQ(*ret_ptr, i);
        }
        
    }

    //collision test

    for (u64 i = 0; i < 512; i++)
    {
        i8* key_ptr = (i8*)&i;
        push_back(&table, key_ptr, &i);
    }

    for (u64 i = 0; i < 512; i++)
    {
        i8* key_ptr = (i8*)(&i);
        hash_table_entry* ret_entry = find(&table, key_ptr);
        slist_t* list = ret_entry->data->head;
        while (nullptr != list)
        {
            u64* ret_ptr = (u64*)list->data;
            list = list->next;
            EXPECT_EQ(*ret_ptr, i);
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

int main(int argc, char** argv) 
{
	prepare_subsystem_for_test();

	::testing::InitGoogleTest(&argc, argv);
	int  ret_val = RUN_ALL_TESTS();
	
	shutdown_test_subsystem();
	return ret_val;
}