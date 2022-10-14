/*****************************************************************//**
 * \file   hash_map.hpp
 * \brief  haku generic hash map
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#pragma once

// TODO : test this

/** key value to denote a free entry */
constexpr const i64 FREE_ENTRY = -1;

/** initial bin size on map creation */
constexpr const u32 INITIAL_BIN_SIZE = 32u;

/** haku hash map entry struct */
typedef  struct hash_map_entry_t
{
	/** key that was entered with */
	i64 key;

	/** data assioated with the key */
	void* data;

}hash_map_entry_t;

/** haku hash map key type */
typedef enum hash_map_key_t
{
	/** enum denoting int key values */
	HASH_MAP_INT_HASH,

	/** enum denoting char hashes */
	HASH_MAP_CHAR_HASH

}hash_map_key_t;

/** haku hash map structure */
typedef struct hash_map_t
{
	/** element count that is entered in the map */
	u32 element_count;

	/** key type and hash function used */
	hash_map_key_t  type;

	/** current bin size */
	u32 bin_size;

	/** element size that is the size of the data in the map */
	u32 element_size;

	/** data bin that holds the data */
	i8* data_bin;

	/** entry bin that holds the hash entry */
	hash_map_entry_t* bin;

}hash_map_t;

/**
 * routine to create a hash map.
 * 
 * \param map pointer to the hash map
 * \param elemnet_size size of the element assioated with the map
 * \param key type that is used it the map
 * \return H_OK on sucess and H_FAIL else
 */
HAPI i8 create_hash_map(hash_map_t* map,u64 element_size,hash_map_key_t key_type);

/**
 * routine to add an entry into the hash map.
 * 
 * \param map pointer to the hash map to make and entry into
 * \param key key to assioate it with
 * \param data pointer to the data that needs to be assioated with the data
 * \return H_OK on sucess
 */
HAPI i8 push_back(hash_map_t* map ,i64 key , void* data);

/**
 * routine to retreive a hash entry relate to the key.
 * 
 * \param map pointer to the map to search for
 * \param key hashed key to look up
 * \return  corresponding entry or nullptr
 */
HAPI hash_map_entry_t* find(hash_map_t* map,i64 key);

/**
 * routine to remove an entry from the hash map.
 * 
 * \param map pointer to the map to remove entry from
 * \param key key assioted with the entry
 * \return H_OK or H_FAIL
 */
HAPI i8 remove_entry(hash_map_t* map,i64 key);

/**
 * routine to get the key with the lower bound.
 * 
 * \param  map pointer to the hash map to be structure
 * \param bound the lower bound
 * \return pointer to the hash_map_entry complying to [lower_bound , infinite)
 */
HAPI hash_map_entry_t* lower_bound(hash_map_t* map, i64 bound);

/**
 * routine to get the key with the upper bound.
 *
 * \param  map pointer to the hash map to be structure
 * \param bound the upper bound
 * \return pointer to the hash_map_entry complying to [ infinite, upper)
 */
HAPI hash_map_entry_t* upper_bound(hash_map_t* map, i64 bound);

/**
 * routine to destroy a hash map properly.
 * 
 * \param  map pointer to the map to destroy
 */
HAPI void destroy_hash_map(hash_map_t* map);




