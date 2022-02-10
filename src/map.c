//
// Created by nik on 2/9/22.
//

#include "map.h"
#include <stdio.h>

inline void __map_resize(Map* map) {
	if (MAP_THRESHOLD <
		(float64) map->count / (float64) map->keypairs.capacity) {
		// Save the old data to copy the elements from.
		Linked_List* old_data = (Linked_List*)map->keypairs.data;
		u64 old_capacity = map->keypairs.capacity;
		// Reset all counters
		map->count = 0;
		map->keypairs.capacity *= 2;
		map->keypairs.count = 0;
		map->keypairs.data = (Linked_List*)
			calloc(map->keypairs.capacity, sizeof (Linked_List));

		// Copy the old data into the newly allocated buffer. Data must be
		// re-inserted because new hashes must be regenerated.
		for (u64 i = 0; i < old_capacity; ++i) {
			Linked_List* curr = old_data + i;
			if (curr->start != NULL) {
				linked_list_foreach_node(curr, struct map_keypair*, {
					map_put(map, it->key, it->value);
					// key and value are dynamically allocated in map_put
					// so, we need to free them.
					free(it->key);
					free(it->value);
				})
			}
		}
		// Lastly, free old data ptr.
		free(old_data);
	}
}

inline void map_new(Map* map, u32 size) {
	list_new(&map->keypairs, sizeof(Linked_List));
	map->count = 0;
	map->size = size;
}

void map_put(Map* map, const char* key, void* data) {
	if (key == NULL) return;
	// Check if the map had reached the threshold for resize.
	__map_resize(map);
	// Generate the index of the key and get the Linked_List corresponding
	// to that index.
	s32 index = __map_get_index(map, key);
	Linked_List* existing_value = ((Linked_List*)
		__list_get_no_bounds_check(&map->keypairs, index));
	// Either of the conditions should mean that the list has not been initialized.
	if (existing_value->count == 0 || existing_value->start == NULL) {
		linked_list_new(existing_value, sizeof(struct map_keypair));
	}

	// We do not want to store duplicate keys. if the key is already present
	// overwrite it.
	linked_list_foreach_node(existing_value, struct map_keypair*, {
		if (strcmp(it->key, key) == 0) {
			memcpy(it->value, data, map->size);
			return;
		}
	})

	// We allocate the buffer to store the copy of the data.
	// This can also be implemented to store the exact pointer provided.
	void* new_data = malloc(map->size);
	memcpy(new_data, data, map->size);
	// @Warning we're duping the key
	struct map_keypair keypair = {.key=strdup(key), .value=new_data};
	linked_list_add(existing_value, &keypair);
	map->count++;
}

inline u64 __map_hash(const char* key) {
	u64 hash = 5381;
	while (*key) {
		hash ^= *key++;
		hash = ((hash << 5u) + hash);
	}

	return hash;
}

void* map_get(Map* map, char* key) {
	if (key == NULL) return NULL;
	s32 index = __map_get_index(map, key);
	// First we find the linked list corresponding to the right
	// index.
	Linked_List* linked_list = ((Linked_List*)
		__list_get_no_bounds_check(&map->keypairs, index));
	if (linked_list->count == 0 && linked_list->start == NULL)
		return NULL;

	// Return the first node with the appropriate key.
	linked_list_foreach_node(linked_list, struct map_keypair*, {
		if (strcmp(it->key, key) == 0) {
			return it->value;
		}
	})
	return NULL;
}

void map_remove(Map* map, char* key) {
	s32 index = __map_get_index(map, key);
	// First we find the linked list corresponding to the right
	// index.
	Linked_List* linked_list =
		__list_get_no_bounds_check(&map->keypairs, index);
	int i = 0;
	// Check if any of the stored nodes match the key. If yes remove it.
	linked_list_foreach_node(linked_list, struct map_keypair*, {
		if (strcmp(it->key, key) == 0) {
			linked_list_remove(linked_list, i);
			map->count--;
			return;
		}
		i++;
	})
}

inline s32 __map_get_index(Map * map, const char* key) {
	u64 hash = __map_hash(key);
	return (s32) (hash % map->keypairs.capacity);
}
