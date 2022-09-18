/*****************************************************************//**
 * \file   hash_set.cpp
 * \brief  hash table allocations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include <string.h>
#include "hash_set.hpp"
#include "core/logger.hpp"
#include "memory/hmemory.hpp"

/** cure */

/**
 * rehashing a hash table since it was close to overflowing.
 * 
 * \param table pointer to table
 */
void rehash(hash_table_t* table);

i8 create_hast_table(hash_table_t* table, u64 element_size, key_type type)
{
    table->element_count = 0;
    table->bin_size = init_bin_size;
    table->element_size = element_size;
    table->type = type;
    table->bin = (hash_table_entry*)hmemory_alloc_zeroed(table->bin_size * sizeof(hash_table_entry), MEM_TAG_HASH_TABLE);

    hash_table_entry* entry = table->bin;

    for (u64 i = 0; i < table->bin_size; i++)
    {
        entry = entry + 1;
        entry->data = nullptr;
        entry->key = UNINITIALIZED_HASH_ENTRY;
    }

    return H_OK;
}

void destroy_hash_table(hash_table_t* table)
{
    hash_table_entry* entry = table->bin;
    for (u64 i = 0; i < table->bin_size; i++)
    {
        entry = table->bin + i;
        if (nullptr != entry->data)
        {
            destroy_slist(entry->data);
            hmemory_free(entry->data,MEM_TAG_HASH_TABLE);
        }
    }
    hmemory_free(table->bin,MEM_TAG_HASH_TABLE);
}

void push_back(hash_table_t* table, i8* key, void* obj)
{
    if ((table->bin_size - table->element_count) < 16)
    {
// TODO :make rehash functions
        rehash(table);
    }

    switch (table->type)
    {
    case INT_HASH:
    {
        u64 hash_val = *(u64*)key;
        u64 hashed_val = hash_key(table, hash_val);
        while (true)
        {
            hash_table_entry* entry_ptr = (table->bin + hashed_val);
            if (NO_HASH_ENTRY == entry_ptr->key)
            {
                push_back(entry_ptr->data, obj);
            }
            else if (nullptr == entry_ptr->data)
            {
                entry_ptr->data = (slist*)hmemory_alloc(sizeof(slist), MEM_TAG_HASH_TABLE);
                entry_ptr->key = hash_val;
                create_slist(entry_ptr->data, table->element_size);
                push_back(entry_ptr->data, obj);
                table->element_count++;
                break;
            }
            else  if (entry_ptr->key == hash_val)
            {
                //collision with the same key
                push_back(entry_ptr->data, obj);
                break;
            }
            else
            {
                //collision with the different key
                hashed_val = hash_key(table, hashed_val);
                continue;
            }
        }

        break;
    }
    default:
        HLEMER("Unknown hash function enum");
        break;
    }
}

hash_table_entry* find(hash_table_t* table, i8* key)
{
    switch (table->type)
    {
    case INT_HASH:
    {
        u64 count = table->element_size;
        u64 hash_val = *(u64*)key;
        u64 hashed_val = hash_key(table, hash_val);
        while (count > 0)
        {
            hash_table_entry* entry_ptr = (table->bin + hashed_val);
            if (entry_ptr->key == hash_val)
            {
                //collision with the same key
                return entry_ptr;
            }
            else
            {
                //collision with the different key
                hashed_val = hash_key(table, hashed_val);
                continue;
            }
            // linear time has ended
            count--;
        }

        // TODO : maybe do a linear search?  and get in 2n complexity
        count = 0;
        while (count < table->element_size)
        {
            hash_table_entry* entry_ptr = (table->bin + count);
            if (entry_ptr->key == hashed_val)
            {
                return entry_ptr;
            }
            count++;
        }
        HLERRO("hash  table hash pair lost..!!");
        return nullptr;
    }
    default:
        return nullptr;
    }
}

i8 remove_entry(hash_table_t* table, i8* key)
{
    hash_table_entry* ret_entry = find(table, key);
    if (nullptr == ret_entry)
    {
        HLERRO("the find returned a lost hash");
        return H_ERR;
    }
    ret_entry->key = NO_HASH_ENTRY;
    reset(ret_entry->data);
    return  H_OK;
}

void rehash(hash_table_t* table)
{
    u64 old_bin_size = table->bin_size;
    hash_table_entry* old_bin = table->bin;
    table->bin_size = 2 * table->bin_size;

    table->bin = (hash_table_entry*)hmemory_alloc(table->bin_size * sizeof(hash_table_entry), MEM_TAG_HASH_TABLE);
    for (u64 i = 0; i < table->bin_size; i++)
    {
        hash_table_entry* entry = table->bin + i;
        entry->data = nullptr;
        entry->key = UNINITIALIZED_HASH_ENTRY;
    }

    for (u64 i = 0; i < old_bin_size; i++)
    {
        hash_table_entry* pos_entry = (old_bin + i);
        if (NO_HASH_ENTRY == pos_entry->key)
        {
            hmemory_copy((table->bin + i), pos_entry, sizeof(hash_table_entry));
        }
        else if (UNINITIALIZED_HASH_ENTRY == pos_entry->key)
        {
            table->bin[i].key = UNINITIALIZED_HASH_ENTRY;
            continue;
        }
        else
        {
            // rehash 
            switch (table->type)
            {
            case INT_HASH:
            {
                u64 hash_val = pos_entry->key;
                u64 hashed_val = hash_key(table, hash_val);
                table->bin[hashed_val].key = pos_entry->key;
                table->bin[hashed_val].data = pos_entry->data;
                break;
            }
            default:
                HLEMER("Unknown hash function enum");
                break;
            }
        }
    }
    // rehashed
    hmemory_free(old_bin, MEM_TAG_HASH_TABLE);
}

void test_hash_table(void)
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
            if (*ret_ptr != i)
            {
                HLEMER("hash corruption");
            }
        }
        else
        {
            HLEMER("hash_entry_data null needs checking");
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
            if (*ret_ptr != i)
            {
                HLEMER("hash corruption for collision");
            }
        }
    }

    HLINFO("Hash test passed");
    destroy_hash_table(&table);
}
