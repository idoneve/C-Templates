#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashmap.h"

void rand_str(char *, size_t);

uint64_t fnv1a_hash(const void *);
uint64_t djb2_hash(const void *);

uint64_t string_compare(const void *, const void *);

void rand_str(char *dest, size_t length) {
    char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (size_t i = 0; i < length; i++) {
        dest[i] = charset[(uint64_t)rand() % (sizeof(charset) - 1)];
    }

    dest[length] = '\0';
}

/* FNV-1a Generic Hash Function */
#define FNV_OFFSET 2166136261U
#define FNV_PRIME 16777619U

uint64_t fnv1a_hash(const void *key) {
    const unsigned char *p = key;
    uint64_t hash = FNV_OFFSET;
    while (*p) {
        hash ^= *p++;
        hash *= FNV_PRIME;
    }
    return hash;
}

/* DJB2 String Hash Function */
uint64_t djb2_hash(const void *key) {
    const char *str = (const char *)key;
    uint64_t hash = 5381;

    uint64_t c;
    while ((c = (uint64_t)*str++)) {
        hash = ((hash << 5) + hash) + c; // ((hash << 5) + hash) + c <=> 33h + c
    }

    return hash;
}

uint64_t string_compare(const void *key1, const void *key2) {
    return strcmp((const char *)key1, (const char *)key2) == 0;
}

int main(void) {
    srand((unsigned int)time(NULL));

    HashMap *map = hashmap_init(128, fnv1a_hash, string_compare);
    if (!map) {
        return 1;
    }

    char keys[1001][65];
    char rand_key[sizeof(keys[0])];
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        rand_str(keys[i], sizeof(keys[i]) - 1);
        if (!hashmap_put(map, keys[i], (void *)i)) {
            hashmap_free(map);
            return 1;
        }

        if ((double)map->size / (double)map->capacity >= 0.75 || i == sizeof(keys) / sizeof(keys[0]) - 1) {
            printf("i = %zu\n", i);
            hashmap_print(map);
            printf("\n\n");
        }

        if (i == 67) {
            strcpy(rand_key, keys[i]);
        }
    }

    printf("rand key = %s\n", rand_key);

    Node *rand_node = hashmap_get(map, rand_key);
    if (rand_node != NULL) {
        printf("rand node val = %d\n", (int64_t)rand_node->value);
    } else {
        printf("rand key not found\n");
    }

    hashmap_free(map);

    return 0;
}