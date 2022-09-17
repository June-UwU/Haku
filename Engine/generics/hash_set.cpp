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
    table->bin = (hash_table_entry*)hmemory_alloc(table->bin_size, MEM_TAG_HASH_TABLE);

    return H_OK;
}

void destroy_hash_table(hash_table_t* table)
{
    for (u64 i = 0; i < table->bin_size; i++)
    {
        if (nullptr != table->bin[i].data)
        {
            destroy_slist(table->bin[i].data);
            hmemory_free(table->bin, MEM_TAG_HASH_TABLE);
        }
    }
    hmemory_free(table->bin,MEM_TAG_HASH_TABLE);
}

void push_back(hash_table_t* table, i8* key, void* obj)
{
    if ((table->element_size - table->element_count) < 16)
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
            if (nullptr == entry_ptr)
            {
                //new key insertion
                entry_ptr = (hash_table_entry*)hmemory_alloc(sizeof(hash_table_entry), MEM_TAG_HASH_TABLE);
                hmemory_copy(entry_ptr->key, key, sizeof(u64));
                entry_ptr->data = (slist*)hmemory_alloc(sizeof(slist), MEM_TAG_HASH_TABLE);
                create_slist(entry_ptr->data, table->element_size);
                push_back(entry_ptr->data, obj);
                break;
            }
            else  if (*(u64*)entry_ptr->key == hashed_val)
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
    case CHAR_HASH:
    {

        u64 hashed_val = hash_key(table, (char*)key);
        while (true)
        {
            hash_table_entry* entry_ptr = (table->bin + hashed_val);
            if (nullptr == entry_ptr)
            {
                entry_ptr = (hash_table_entry*)hmemory_alloc(sizeof(hash_table_entry), MEM_TAG_HASH_TABLE);
                hmemory_copy(entry_ptr->key, key, strlen((char*)key));
                entry_ptr->data  = (slist*)hmemory_alloc(sizeof(slist), MEM_TAG_HASH_TABLE);
                create_slist(entry_ptr->data, table->element_size);
                push_back(entry_ptr->data, obj);
                break;
            }
            else  if (0 == strcmp((char*)entry_ptr->key,(char*)key))
            {
                push_back(entry_ptr->data, obj);
                break;
            }
            else
            {
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
            if (nullptr == entry_ptr)
            {
                // removed hash entry case
                hashed_val = hash_key(table, hashed_val);
                continue;
            }
            else  if (*(u64*)entry_ptr->key == hashed_val)
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
            if (*(u64*)entry_ptr->key == hashed_val)
            {
                return entry_ptr;
            }
            count++;
        }
        HLERRO("hash  table hash pair lost..!!");
        return nullptr;
    }
    case CHAR_HASH:
    {
        u64 count = table->element_size;
        u64 hashed_val = hash_key(table, key);
        while (count > 0)
        {
            hash_table_entry* entry_ptr = (table->bin + hashed_val);
            if (nullptr == entry_ptr)
            {
                // removed hash entry case
                hashed_val = hash_key(table, hashed_val);
                continue;
            }
            else  if (0 == strcmp((char*)entry_ptr->key, (char*)key))
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
            if (*(u64*)entry_ptr->key == hashed_val)
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
    hash_table_entry* temp = ret_entry;
    ret_entry = nullptr;
    hmemory_free(temp, MEM_TAG_HASH_TABLE);
    return  H_OK;
}

void rehash(hash_table_t* table)
{
    u64 old_bin_size = table->bin_size;
    hash_table_entry* old_bin = table->bin;
    table->bin_size = 2 * table->bin_size;

    for (u64 i = 0; i < old_bin_size; i++)
    {
        hash_table_entry* pos_entry = (old_bin + i);
        if (nullptr == pos_entry)
        {
            continue;
        }
        else
        {
            // rehash 
            push_back(table, pos_entry->key, pos_entry->data);
        }
    }
    // rehashed
}
