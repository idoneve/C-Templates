#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashmap.h"

uint64_t int_hash(const void *key);
uint64_t int_compare(const void *key1, const void *key2);

uint64_t int_hash(const void *key) {
    const int64_t n = (const int64_t)key;
    int64_t hash = 78437;

    return 7 * hash * (hash + (hash << 5)) + 17 * hash + 11 * n;
}

uint64_t int_compare(const void *key1, const void *key2) {
    return (const int64_t)key1 == (const int64_t)key2;
}

char keys[1000001][2];

int main(void) {
    srand((unsigned int)time(NULL));

    HashMap *map = hashmap_init(128, int_hash, int_compare);
    if (!map) {
        return 1;
    }

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        if (!hashmap_put(map, (void *)(int64_t)rand(), (void *)i)) {
            hashmap_free(map);
            return 1;
        }

        if (i % 1000 == 0 && i != 0) {
            printf("i = %zu\n", i);
            hashmap_print(map);
            printf("\n\n");
        }
    }

    hashmap_free(map);

    return 0;
}
