#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashmap.h"

void rand_str(char *dest, size_t length) {
    char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (size_t i = 0; i < length; i++) {
        dest[i] = charset[rand() % (sizeof(charset) - 1)];
    }

    dest[length] = '\0';
}

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

uint64_t string_compare(const void *key1, const void *key2) {
    return strcmp((const char *)key1, (const char *)key2) == 0;
}

int main(void) {
    srand((unsigned int)time(NULL));

    HashMap *map = hashmap_init(128, string_hash, string_compare);
    if (!map) {
        return 1;
    }

    char keys[rand() % 1000][65];
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        rand_str(keys[i], sizeof(keys[i]) - 1);
        if (!hashmap_put(map, keys[i], NULL)) {
            hashmap_free(map);
            return 1;
        }

        if (((double)map->size / (double)map->capacity) >= 0.75) {
            printf("i = %zu\n", i);
            hashmap_print(map);
            printf("\n\n");
        }
    }

    hashmap_free(map);

    return 0;
}
