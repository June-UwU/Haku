/*****************************************************************//**
 * \file   hash_set.hpp
 * \brief  haku hash set , internally conflicts are resolved based on key matching and has some template elements 
 * 
 * \see	   https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include "generics/single_ll.hpp"

/**
 * macros to create a hash table using int hash.
 * 
 * \param table pointer to hash table
 * \param obj objects in hash table
 */
#define HAKU_CREATE_INT_HASH_TABLE(ptable,obj) create_hast_table(ptable,sizeof(obj), INT_HASH)

 /**
  * macros to create a hash table using char hash.
  *
  * \param table pointer to hash table
  * \param obj objects in hash table
  */
#define HAKU_CREATE_CHAR_HASH_TABLE(ptable,obj)  create_hast_table(ptable,sizeof(obj), CHAR_HASH)


/** constant for Cormen's multiplication hash method */
constexpr const f32 A = 0.6180339887f; //(A is some random looking real number, the Knuth variable)

/** constant to signify that the hash is uninitialized */
constexpr const i64 UNINITIALIZED_HASH_ENTRY = -2;

/** constant to signify that the hash is free */
constexpr const i64 NO_HASH_ENTRY = -1;

/** hash bin initial size */
constexpr const u64 init_bin_size = 32u; //()

/** a hash table entry */
typedef struct hash_table_entry
{
	/** key */
	i64 key;

	/** list that is hashed with the key */
	slist* data;
}hash_table_entry;

/** enum to point with hash function to use */
typedef enum  key_type
{
	/** interger hash */
	INT_HASH,

// TODO : make a character hash maps
	/** character hash */
	//CHAR_HASH
}key_type;

/** hash table structure */
typedef struct hash_table_t
{
	/** element size */
	u64 element_count;

	/** key type used */
	key_type type;

	/** element size in the hash table */
	u64 element_size;

	/** current bin size */
	u64 bin_size;

	/** pointer to the bin */
	hash_table_entry* bin;
}hash_table_t;

/**
 * routine to create a hash table.
 * 
 * \param table point to the hash table to be initialized
 * \param element_size hash chain element_size
 * \param type type of key to be hashed (chooses the hash function)
 * \return H_OK on sucess
 */
HAPI i8 create_hast_table(hash_table_t* table,u64 element_size, key_type type);

/**
 * routine to destroy a hash table properly.
 * 
 * \param  table pointer to hash table
 */
HAPI void destroy_hash_table(hash_table_t* table);

/**
 * routine to a hash_table_entry with a hashed key.
 * 
 * \param table  pointer to hash table
 * \param key pointer to the key
 * \param pointer to the object
 */
HAPI void push_back(hash_table_t* table , void* key, void* obj);

/**
 * routine to retreive the hash table entry.
 * 
 * \param table pointer to the hash table
 * \param key key to do look up on
 * \return hash_table_entry on sucess or nullptr on failure
 */
HAPI hash_table_entry* find(hash_table_t* table, void* key);

/**
 * routine to remove an entry from the hash table.
 * 
 * \param table pointer to hash table
 * \param key key to look up and remove element on
 * \return H_OK on sucess
 */
HAPI i8 remove_entry(hash_table_t* table, void* key);

// TODO : make integer hashing not use casts
/**
 * hashing functions.
 * 
 * \param table pointer to table
 * \param key key given
 * \return u64 hash of the value under [0,table->bin_size)
 */
template <typename T>
u64 hash_key(hash_table_t* table,T key)
{
	f32 S = key * A;
	i32 num = S;
	f32 X = S - num;
	return table->bin_size * X;
}

/**
 * hashing functions for strings.
 * 
 * \param table table pointer to table
 * \param ptr key given
 * \return u64 hash of the value under [0,table->bin_size)
 */
template<char*>
u64 hash_key(hash_table_t* table, char* ptr)
{
	u64 sum = 0;
	while (0 != ptr)
	{
		sum += *ptr; // ptr++ readability..??
		ptr++;
	}
	return sum % table->bin_size;
}

/** routine to test hash table */
void test_hash_table(void);