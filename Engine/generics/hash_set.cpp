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
void rehash(hash_set_t * table);

i8 create_hast_table(hash_set_t * table, u64 element_size, key_type type)
{
    table->element_count = 0;
    table->bin_size = init_bin_size;
    table->element_size = element_size;
    table->type = type;
    table->bin = (hash_set_entry_t *)hmemory_alloc_zeroed(table->bin_size * sizeof(hash_set_entry_t ), MEM_TAG_HASH_TABLE);

    hash_set_entry_t * entry = table->bin;

    for (u64 i = 0; i < table->bin_size; i++)
    {
        entry->data = nullptr;
        entry->key = UNINITIALIZED_HASH_ENTRY;
        entry = entry + 1;
    }

    return H_OK;
}

void destroy_hash_table(hash_set_t * table)
{
    hash_set_entry_t * entry = table->bin;
    for (u64 i = 0; i < table->bin_size; i++)
    {
        entry = table->bin + i;
        if (nullptr != entry->data)
        {
            destroy_queue(entry->data);
            hmemory_free(entry->data,MEM_TAG_HASH_TABLE);
        }
    }
    hmemory_free(table->bin,MEM_TAG_HASH_TABLE);
}

void push_back(hash_set_t * table, void* key, void* obj)
{
    if ((table->bin_size - table->element_count) < 16u)
    {
        rehash(table);
    }

    switch (table->type)
    {
    case INT_HASH:
    {
        u64 hash_val = *(u64*)key;
        u64 hashed_val = hash_key(table, hash_val);
        

        hash_set_entry_t * entry_ptr = (table->bin + hashed_val);
        if (NO_HASH_ENTRY == entry_ptr->key)
        {
            enqueue(entry_ptr->data, obj);
        }
        else if (nullptr == entry_ptr->data)
        {
            entry_ptr->data = (queue_t*)hmemory_alloc(sizeof(queue_t), MEM_TAG_HASH_TABLE);
            entry_ptr->key = hash_val;
            create_queue(entry_ptr->data, table->element_size);
            enqueue(entry_ptr->data, obj);
            table->element_count++;
        }
        else  if (entry_ptr->key == hash_val)
        {
            //collision with the same key
            enqueue(entry_ptr->data, obj);
        }
        else
        {
            //collision with the different key
            while (true)
            {
                // no out of range hashes
                hashed_val++;
                hashed_val = hashed_val % table->bin_size;
                hash_set_entry_t * entry_ptr = (table->bin + hashed_val);
                if (entry_ptr->key == hash_val)
                {
                    entry_ptr->key = hash_val;
                    enqueue(entry_ptr->data, obj);
                }
                else if (NO_HASH_ENTRY == entry_ptr->key)
                {
                    enqueue(entry_ptr->data, obj);
                    break;
                }
                else if (nullptr == entry_ptr->data)
                {
                    entry_ptr->data = (queue_t*)hmemory_alloc(sizeof(queue_t), MEM_TAG_HASH_TABLE);
                    entry_ptr->key = hash_val;
                    create_queue(entry_ptr->data, table->element_size);
                    enqueue(entry_ptr->data, obj);
                    table->element_count++;
                    break;
                }
                else
                {
                    continue;
                }
            }
        }

        break;
    }
    default:
        HLEMER("Unknown hash function enum");
        break;
    }
}

hash_set_entry_t * find(hash_set_t * table, void* key)
{

    switch (table->type)
    {
    case INT_HASH:
    {
        u64 count = table->bin_size;
        u64 hash_val = *(u64*)key;
        u64 hashed_val = hash_key(table, hash_val);
        while (count > 0)
        {
            hash_set_entry_t * entry_ptr = (table->bin + hashed_val);
            if (entry_ptr->key == hash_val)
            {
                //collision with the same key
                return entry_ptr;
            }
            else
            {
                //collision with the different key
                hashed_val++;
                hashed_val = hashed_val % table->bin_size;
                count--;
                continue;
            }
            // linear time has ended
        }
        return nullptr;
    }
    default:
        return nullptr;
    }
}

i8 remove_entry(hash_set_t * table, void* key)
{
    hash_set_entry_t * ret_entry = find(table, key);
    if (nullptr == ret_entry)
    {
        HLERRO("the find returned a lost hash");
        return H_ERR;
    }
    ret_entry->key = NO_HASH_ENTRY;
    reset(ret_entry->data);
    return  H_OK;
}

hash_set_entry_t* next(hash_set_t* table, hash_set_entry_t* entry)
{
    u64 offset = table->bin - entry;
    offset = offset / table->element_size;
    offset = offset + 1;
    if (offset >= table->bin_size)
    {
        return nullptr;
    }
    return table->bin + offset;
}

void rehash(hash_set_t * table)
{
    HLWARN("Rehashed hash table : %x ", table);
    u64 old_bin_size = table->bin_size;
    hash_set_entry_t * old_bin = table->bin;
    table->bin_size = 2 * table->bin_size;

    table->bin = (hash_set_entry_t *)hmemory_alloc_zeroed(table->bin_size * sizeof(hash_set_entry_t ), MEM_TAG_HASH_TABLE);
    for (u64 i = 0; i < table->bin_size; i++)
    {
        hash_set_entry_t * entry = table->bin + i;
        entry->data = nullptr;
        entry->key = UNINITIALIZED_HASH_ENTRY;
    }
// TODO : make it more nicer..? this is ungy to look at for handling initialized previous entries
    for (u64 i = 0; i < old_bin_size; i++)
    {
        hash_set_entry_t * pos_entry = (old_bin + i);
        if (NO_HASH_ENTRY == pos_entry->key)
        {
            auto table_entry = table->bin + i;
            table_entry->key = NO_HASH_ENTRY;
            table_entry->data = pos_entry->data;
        }
    }

    for (u64 i = 0; i < old_bin_size; i++)
    {
        hash_set_entry_t * pos_entry = (old_bin + i);


        if (NO_HASH_ENTRY == pos_entry->key)
        {
            continue;
        }
        else if (UNINITIALIZED_HASH_ENTRY == pos_entry->key)
        {
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

                while (true)
                {
                    hash_set_entry_t * entry_ptr = (table->bin + hashed_val);
                    if (NO_HASH_ENTRY == entry_ptr->key)
                    {
                        entry_ptr->key = pos_entry->key;
                        enqueue(entry_ptr->data, pos_entry->data);
                        break;
                    }
                    else if (nullptr == entry_ptr->data)
                    {
                        entry_ptr->key = hash_val;
                        entry_ptr->data = pos_entry->data;
                        table->element_count++;
                        break;
                    }
                    else  if (entry_ptr->key == hash_val)
                    {
                        // this shouldn't be a case ..? if the hash is proper
                        //collision with the same key
                        entry_ptr->key = pos_entry->key;
                        enqueue(entry_ptr->data, pos_entry->data);
                        hmemory_free(pos_entry->data, MEM_TAG_HASH_TABLE);
                        break;
                    }
                    else
                    {
                        //collision with the different key
                        while (true)
                        {
                            // no out of range hashes
                            hashed_val++;
                            hashed_val = hashed_val % table->bin_size;
                            hash_set_entry_t * entry_ptr = (table->bin + hashed_val);
                            if (NO_HASH_ENTRY == entry_ptr->key)
                            {
                                entry_ptr->key = pos_entry->key;
                                enqueue(entry_ptr->data, pos_entry->data);
                                hmemory_free(pos_entry->data, MEM_TAG_HASH_TABLE);
                                break;
                            }
                            else if (nullptr == entry_ptr->data)
                            {
                                entry_ptr->data = pos_entry->data;
                                entry_ptr->key = pos_entry->key;
                                table->element_count++;
                                break;
                            }
                            else
                            {
                                continue;
                            }
                        }
                        break;
                    }
                }
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
        hash_set_entry_t * ret_entry = find(&table, &i);
        if (nullptr != ret_entry)
        {
            u64* ret_ptr = (u64*)front(ret_entry->data)->data;
            if (*ret_ptr != i)
            {
                HLEMER("hash corruption");
                passed = false;
            }
        }
        else
        {
            HLEMER("entry lost \n \t KEY : %d \n \t HASH : %d", i, hash_key(&table, i));
            passed = false;
        }
    }

    //collision test

    for (u64 i = 0; i < test_range; i++)
    {
        push_back(&table, &i, &i);
    }
    
    for (u64 i = 0; i < test_range; i++)
    {
        hash_set_entry_t * ret_entry = find(&table, &i);
        queue_t* list = ret_entry->data;
        for_queue_t(list,entry->next != NULL_PTR)
        {
            u64* ret_ptr = (u64*)entry->data;
            if (*ret_ptr != i)
            {
                HLEMER("hash corruption for collision");
                passed = false;
            }
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
            hash_set_entry_t * ret_entry = find(&table, &i);
            if (ret_entry != nullptr)
            {
                HLEMER("junk value returned..! \n\t KEY : %d \n\t HASH : %d", i, hash_key(&table, i));
                passed = false;
            }
        }
    }

    if(true == passed)
    {
        HLINFO("Hash table test passed");
    }
    else 
    {
        HLCRIT("Hash table test failure");
    }

    destroy_hash_table(&table);
}