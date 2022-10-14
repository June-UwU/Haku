/*****************************************************************//**
 * \file   hash_map.cpp
 * \brief  haku generic hash map implementations
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "defines.hpp"
#include "hash_map.hpp"
#include "memory/hmemory.hpp"
#include "core/logger.hpp"

 /** constant that triggers a rehash when the leftover size is below this */
constexpr const u32 REHASH_SIZE = 16u;

 /** constant for Cormen's multiplication hash method */
constexpr const f32 A = 0.6180339887f; //(A is some random looking real number, the Knuth variable)

/**
 * routine to rehash a hash_map.
 * 
 * \param map pointer to a hash map to rehash
 */
void rehash(hash_map_t* map);

/**
 * rouitne to properly enter data into a rehashed hash map.
 * 
 * \param map pointer to the map to make entry to
 * \param key key to make hash entry
 * \param data data assioated with the key
 * \return H_OK on sucess and H_FAIL else
 */
i8 make_entry(hash_map_t* map, i64 key, void* data);

/**
 * integer hashing function.
 * 
 * \param map pointer to hash map to make the entry in
 * \param key key to hash
 * \return 
 */
u64 hash_key_int(hash_map_t* table, i64 key);

i8 create_hash_map(hash_map_t* map, u64 element_size, hash_map_key_t key_type)
{
	map->element_size = element_size;
	map->type = key_type;
	map->bin_size = INITIAL_BIN_SIZE;
	map->element_count = 0;
	map->bin = (hash_map_entry_t*)hmemory_alloc(INITIAL_BIN_SIZE * sizeof(hash_map_entry_t), MEM_TAG_HASH_MAP);
	map->data_bin = (i8*)hmemory_alloc(INITIAL_BIN_SIZE * element_size, MEM_TAG_HASH_MAP);

	hmemory_set(map->bin, FREE_ENTRY, INITIAL_BIN_SIZE * sizeof(hash_map_entry_t));

	return H_OK;
}

i8 push_back(hash_map_t* map,i64 key, void* data)
{
	if ((map->bin_size - map->element_count) < REHASH_SIZE)
	{
		rehash(map);
	}

	switch (map->type)
	{
	case HASH_MAP_INT_HASH:
	{
		u64 hashed_val = hash_key_int(map, key);

		hash_map_entry_t* entry = (map->bin + hashed_val);
		if (entry->key == FREE_ENTRY)
		{
			entry->key = key;
			entry->data = (map->data_bin + (map->element_size * (hashed_val % map->bin_size)));
			hmemory_copy(entry->data, data, map->element_size);
			map->element_count++;
		}
		else
		{
			while (true)
			{
				hashed_val += 1;
				entry = (map->bin + (hashed_val % map->bin_size));
				if (entry->key == FREE_ENTRY)
				{
					entry->key = key;
					entry->data = map->data_bin + (map->element_size * (hashed_val % map->bin_size));
					hmemory_copy(entry->data, data, map->element_size);
					map->element_count++;
					return H_OK;
				}
			}
		}

		break;
	}
	case HASH_MAP_CHAR_HASH:
	{
		HLEMER("char hash map is not yet implemented...!");
		return H_FAIL;
	}
	default:
		HLEMER("unknown hash map type...!");
		return H_FAIL;
	}
	return H_FAIL;
}

hash_map_entry_t* find(hash_map_t* map, i64 key)
{
	switch (map->type)
	{
	case HASH_MAP_INT_HASH:
	{
		u64 hashed_val = hash_key_int(map, key);

		hash_map_entry_t* entry = (map->bin + (hashed_val % map->bin_size));
		if (entry->key == key)
		{
			entry->key = key;
			entry->data = map->data_bin + (map->element_size * (hashed_val % map->bin_size));
			entry = (map->bin + (hashed_val % map->bin_size));
			return entry;
		}
		else
		{
			u32 count = 0;
			while (true)
			{
				hashed_val += 1;
				entry = (map->bin + (hashed_val % map->bin_size));
				if (entry->key == key)
				{
					entry = (map->bin + hashed_val);
					return entry;
				}
				if (count > map->bin_size)
				{
					//HLCRIT("unable to find the hash map entry, THIS MIGHT NOT BE AN ERROR");
					return nullptr;
				}
				count++;
			}
		}
		break;
	}
	case HASH_MAP_CHAR_HASH:
	{
		HLEMER("char hash map is not yet implemented...!");
		return nullptr;
	}
	default:
		HLEMER("unknown hash map type...!");
		return nullptr;
	}
	return nullptr;
}

i8 remove_entry(hash_map_t* map, i64 key)
{
	switch (map->type)
	{
	case HASH_MAP_INT_HASH:
	{
		u64 hashed_val = hash_key_int(map, key);

		hash_map_entry_t* entry = (map->bin + (hashed_val % map->bin_size));
		if (entry->key == key)
		{
			entry->key = key;
			entry->data = map->data_bin + (map->element_size * (hashed_val % map->bin_size));
			entry = (map->bin + (hashed_val % map->bin_size));
			entry->key = FREE_ENTRY;
			map->element_count--;
			return H_OK;
		}
		else
		{
			while (true)
			{
				hashed_val += 1;
				entry = (map->bin + (hashed_val % map->bin_size));
				if (entry->key == key)
				{
					entry = (map->bin + hashed_val);
					entry->key = FREE_ENTRY;		
					map->element_count--;
					return H_OK;
				}
			}
		}
		break;
	}
	case HASH_MAP_CHAR_HASH:
	{
		HLEMER("char hash map is not yet implemented...!");
		return H_FAIL;
	}
	default:
		HLEMER("unknown hash map type...!");
		return H_FAIL;
	}
	return H_FAIL;
}

hash_map_entry_t* lower_bound(hash_map_t* map, i64 bound)
{
	u64 i = 0u;
	hash_map_entry_t* ret_ptr = nullptr;
	hash_map_entry_t* comp = map->bin;
	while (true)
	{
		if (i >= map->bin_size)
		{
			return nullptr;
		}

		if (comp->key >= bound && FREE_ENTRY != comp->key)
		{
			ret_ptr = (map->bin + i);
			break;
		}
		i++;
		comp = (map->bin + i);
	}

	for (; i < map->bin_size; i++)
	{
		comp = (map->bin + i);
		if (comp->key >= bound && comp->key < ret_ptr->key && FREE_ENTRY!= comp->key)
		{
			ret_ptr = comp;
		}
	}

	return ret_ptr;
}

hash_map_entry_t* upper_bound(hash_map_t* map, i64 bound)
{
	u64 i = 0u;
	hash_map_entry_t* ret_ptr = nullptr;
	hash_map_entry_t* comp = map->bin;
	while (true)
	{
		if (i >= map->bin_size)
		{
			return nullptr;
		}

		if (comp->key < bound && FREE_ENTRY != comp->key)
		{
			ret_ptr = (map->bin + i);
			break;
		}
		i++;
		comp = (map->bin + i);
	}

	for (; i < map->bin_size; i++)
	{
		comp = (map->bin + i);
		if (comp->key < bound && comp->key > ret_ptr->key && FREE_ENTRY != comp->key)
		{
			ret_ptr = comp;
		}
	}

	return ret_ptr;
}

void destroy_hash_map(hash_map_t* map)
{
	hmemory_free(map->bin, MEM_TAG_HASH_MAP);
	hmemory_free(map->data_bin, MEM_TAG_HASH_MAP);
}


u64 hash_key_int(hash_map_t* map, i64 key)
{
	f32 S = key * A;
	i32 num = S;
	f32 X = S - num;
	return map->bin_size * X;
}

// TODO : rehash 
void rehash(hash_map_t* map)
{
	i8* old_data_bin = map->data_bin;
	hash_map_entry_t* old_hash_entry = map->bin;
	u32 old_bin_size = map->bin_size;

	map->bin_size = map->bin_size * 2;
	map->data_bin = (i8*)hmemory_alloc(map->bin_size * map->element_size, MEM_TAG_HASH_MAP);
	map->bin = (hash_map_entry_t*)hmemory_alloc(map->bin_size * sizeof(hash_map_entry_t), MEM_TAG_HASH_MAP);
	map->element_count = 0;

	for (u32 i = 0; i < map->bin_size; i++)
	{
		map->bin[i].key = FREE_ENTRY;
		map->bin[i].data = nullptr;
	}

	for (u32 i = 0; i < old_bin_size; i++)
	{
		if (FREE_ENTRY != old_hash_entry[i].key)
		{
			make_entry(map, old_hash_entry[i].key, old_hash_entry[i].data);
		}
	}
	HLWARN("generic hash map rehash\n\t bin size : %d\n\t old bin : %p\n\t new bin : %p\n\t old data : %p\n\t new data : %p", 
		map->bin_size, old_hash_entry, map->bin, old_data_bin, map->data_bin);
	hmemory_free(old_hash_entry, MEM_TAG_HASH_MAP);
	hmemory_free(old_data_bin, MEM_TAG_HASH_MAP);
}


i8 make_entry(hash_map_t* map, i64 key, void* data)
{
	switch (map->type)
	{
	case HASH_MAP_INT_HASH:
	{
		u64 hashed_val = hash_key_int(map, key);

		hash_map_entry_t* entry = (map->bin + hashed_val);
		if (entry->key == FREE_ENTRY)
		{
			entry->key = key;
			entry->data = (map->data_bin + (map->element_size * (hashed_val % map->bin_size)));
			hmemory_copy(entry->data, data, map->element_size);
			map->element_count++;
		}
		else
		{
			while (true)
			{
				hashed_val += 1;
				entry = (map->bin + (hashed_val % map->bin_size));
				if (entry->key == FREE_ENTRY)
				{
					entry->key = key;
					entry->data = map->data_bin + (map->element_size * (hashed_val % map->bin_size));
					hmemory_copy(entry->data, data, map->element_size);
					map->element_count++;
					return H_OK;
				}
			}
		}

		break;
	}
	case HASH_MAP_CHAR_HASH:
	{
		HLEMER("char hash map is not yet implemented...!");
		return H_FAIL;
	}
	default:
		HLEMER("unknown hash map type...!");
		return H_FAIL;
	}
	return H_FAIL;
}
