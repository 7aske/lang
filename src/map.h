#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
//
// Created by nik on 2/9/22.
//

#ifndef LANG_MAP_H
#define LANG_MAP_H

#include "list.h"
#include "linked_list.h"

#define MAP_THRESHOLD        0.6

/**
 * Struct representing an entry in the Map.
 */
struct map_keypair {
	const char* key;
	void*       value;
};

/**
 * Struct representing a HashTable or a HashMap structure.
 *
 * @param keypairs List of Linked_List structs containing the elements
 *                 of the Map in form of a struct map_keypair.
 * @param size Size in bytes of the stored element.
 * @param count Count of elements stored in the Map.
 */
typedef struct map {
	List keypairs;
	u32  size;
	u32  count;
} Map;

/**
 * Initializes a new Map struct.
 *
 * @param map  Location of where to initialize the Map.
 * @param size Size of a single Map entry.
 */
void map_new(Map* map, u32 size);

/**
 * Retrieves an element corresponding to the key.
 *
 * @param map this.
 * @param key Key of the element to retrieve.
 * @return Found element or NULL.
 */
void* map_get(Map* map, char* key);

/**
 * Removes the element matching the key.
 *
 * @param map this.
 * @param key Key of the element to be removed.
 */
void  map_remove(Map* map, char* key);

/**
 * Puts the element into to Map.
 *
 * @param map this.
 * @param key  Key of the element to be put in the Map.
 * @param data Pointer to the value of the element to be put into the Map.
 */
void map_put(Map* map, const char* key, void* data);

/**
 * Utility function to produce the hash used to index into the keypair list.
 *
 * @param key String key to produce the hash for.
 * @return Number representing the hashed key.
 */
u64 __map_hash(const char* key);

/**
 * Utility wrapper to return the corresponding index for the element with key.
 *
 * @param map this.
 * @param key String key to produce the index(hash) for.
 * @return Number representing the Map  index of the key.
 */
s32 __map_get_index(Map* map, const char* key);

/**
 * Resizes the map keeping re-inserting all the elements.
 *
 * @param map this.
 */
void __map_resize(Map* map);

#endif //LANG_MAP_H

#pragma clang diagnostic pop