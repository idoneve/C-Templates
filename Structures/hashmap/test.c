#include <stdio.h>
#include <string.h>

#include "hashmap.h"

/* DJB2 String Hash Function */
uint64_t string_hash(const void *key) {
    const char *str = (const char *)key;
    uint64_t hash = 5381;

    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

int64_t string_compare(const void *key1, const void *key2) {
    return strcmp((const char *)key1, (const char *)key2) == 0;
}

int main(void) {
    HashMap *map = hashmap_init(4, string_hash, string_compare);
    if (!map) {
        return 1;
    }

    int val1 = 100, val2 = 200, val3 = 300, val_update = 999;

    hashmap_put(map, "apple", &val1);
    hashmap_put(map, "banana", &val2);
    hashmap_put(map, "cherry", &val3);
    hashmap_put(map, "apple", &val_update);

    printf("Size: %llu, Capacity: %llu\n\n", map->size, map->capacity);

    const Node *node_apple = hashmap_get(map, "apple");
    const Node *node_banana = hashmap_get(map, "banana");
    const Node *node_missing = hashmap_get(map, "orange");

    if (node_apple) {
        printf("Found 'apple': %d\n", *(int *)node_apple->value);
    }

    if (node_banana) {
        printf("Found 'banana': %d\n", *(int *)node_banana->value);
    }

    if (!node_missing) {
        printf("'orange' correctly not found (NULL)\n\n");
    }

    uint64_t del_status = hashmap_delete(map, "banana");
    printf("Deleted 'banana' status: %llu\n", del_status);

    const Node *node_banana_check = hashmap_get(map, "banana");
    if (!node_banana_check) {
        printf("'banana' confirm deleted!\n");
    }
    printf("New Map Size: %llu\n\n", map->size);

    hashmap_free(map);
    return 0;
}
