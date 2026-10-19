#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint64_t(*HashFunc)(const void *key);
typedef uint64_t(*CompareFunc)(const void *key1, const void *key2);

typedef struct Node Node;
typedef struct HashMap HashMap;

HashMap *hashmap_init(uint64_t, HashFunc, CompareFunc);

void hashmap_free(HashMap *);

uint64_t hashmap_put(HashMap *, void *, void *);
uint64_t hashmap_delete(HashMap *, const void *);
uint64_t hashmap_resize(HashMap *, uint64_t);

const Node *hashmap_get(HashMap *, const void *);

void hashmap_print(HashMap *);

struct Node {
    void *key;
    void *value;

    Node *next;
};

struct HashMap {
    Node **buckets;
    uint64_t capacity;
    uint64_t size;

    HashFunc hash;
    CompareFunc compare;
};

HashMap *hashmap_init(const uint64_t capacity, HashFunc h_func, CompareFunc c_func) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL;
    }

    map->capacity = capacity;
    map->size = 0;

    map->hash = h_func;
    map->compare = c_func;

    map->buckets = (Node **)calloc(capacity, sizeof(Node *));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    return map;
}

void hashmap_free(HashMap *map) {
    if (map == NULL) {
        return;
    }

    for (uint64_t i = 0; i < map->capacity; ++i) {
        Node *curr_node = map->buckets[i];
        while (curr_node != NULL) {
            Node *next_node = (Node *)curr_node->next;
            free(curr_node);
            curr_node = next_node;
        }
    }

    free(map->buckets);
    free(map);
}

uint64_t hashmap_put(HashMap *map, void *key, void *value) {
    if (map == NULL) {
        return 0;
    }

    if ((double)map->size / (double)map->capacity >= 0.75) {
        hashmap_resize(map, map->capacity * 2);
    }

    uint64_t raw_hash = map->hash(key);
    uint64_t index = raw_hash % map->capacity;

    Node *curr_node = map->buckets[index];
    while (curr_node != NULL) {
        if (map->compare(curr_node->key, key)) {
            curr_node->value = value;
            return 1;
        }

        curr_node = (Node *)curr_node->next;
    }

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        return 0;
    }

    new_node->key = key;
    new_node->value = value;
    new_node->next = map->buckets[index];

    map->buckets[index] = new_node;
    ++map->size;

    return 1;
}

uint64_t hashmap_delete(HashMap *map, const void *key) {
    if (map == NULL) {
        return 0;
    }

    uint64_t raw_hash = map->hash(key);
    uint64_t index = raw_hash % map->capacity;

    Node *curr_node = map->buckets[index];
    Node *prev_node = NULL;

    while (curr_node != NULL) {
        if (map->compare(curr_node->key, key)) {
            if (prev_node == NULL) {
                map->buckets[index] = (Node *)curr_node->next;
            } else {
                prev_node->next = curr_node->next;
            }

            free(curr_node);
            --map->size;

            return 1;
        }

        prev_node = curr_node;
        curr_node = (Node *)curr_node->next;
    }

    return 0;
}

uint64_t hashmap_resize(HashMap *map, const uint64_t new_capacity) {
    if (map == NULL) {
        return 0;
    }

    Node **new_buckets = (Node **)calloc(new_capacity, sizeof(Node *));
    if (new_buckets == NULL) {
        return 0;
    }

    for (uint64_t i = 0; i < map->capacity; ++i) {
        Node *curr_node = map->buckets[i];
        while (curr_node != NULL) {
            Node *next_node = curr_node->next;

            uint64_t raw_hash = map->hash(curr_node->key);
            uint64_t new_index = raw_hash % new_capacity;

            curr_node->next = new_buckets[new_index];
            new_buckets[new_index] = curr_node;

            curr_node = next_node;
        }
    }

    free(map->buckets);

    map->buckets = new_buckets;
    map->capacity = new_capacity;

    return 1;
}

const Node *hashmap_get(HashMap *map, const void *key) {
    if (map == NULL) {
        return NULL;
    }

    uint64_t raw_hash = map->hash(key);
    uint64_t index = raw_hash % map->capacity;

    Node *curr_node = map->buckets[index];
    while (curr_node != NULL) {
        if (map->compare(curr_node->key, key)) {
            return curr_node;
        }

        curr_node = (Node *)curr_node->next;
    }

    return NULL;
}

void hashmap_print(HashMap *map) {
    if (map == NULL) {
        printf("HashMap: (null)\n");
        return;
    }

    if (map->capacity == 0) {
        printf("HashMap: capacity is zero\n");
        return;
    }

    uint64_t occupied_buckets = 0;
    uint64_t max_chain_length = 0;

    for (uint64_t i = 0; i < map->capacity; ++i) {
        uint64_t chain_length = 0;
        for (const Node *node = map->buckets[i]; node != NULL; node = node->next) {
            ++chain_length;
        }

        if (chain_length > 0) {
            ++occupied_buckets;
        }
        if (chain_length > max_chain_length) {
            max_chain_length = chain_length;
        }
    }

    printf("HashMap: size=%" PRIu64 ", capacity=%" PRIu64
        ", load=%.1f%%, occupied buckets=%" PRIu64
        ", max chain=%" PRIu64 "\n",
        map->size, map->capacity,
        100.0 * (double)map->size / (double)map->capacity,
        occupied_buckets, max_chain_length);

    for (uint64_t start = 0; start < map->capacity; start += 32) {
        uint64_t end = start + 32;
        if (end > map->capacity) {
            end = map->capacity;
        }

        printf("%3" PRIu64 "-%3" PRIu64 " | ", start, end - 1);
        for (uint64_t i = start; i < end; ++i) {
            uint64_t chain_length = 0;
            for (const Node *node = map->buckets[i]; node != NULL; node = node->next) {
                ++chain_length;
            }

            char marker = '.';
            if (chain_length >= 1 && chain_length <= 9) {
                marker = (char)('0' + chain_length);
            } else if (chain_length >= 10 && chain_length <= 35) {
                marker = (char)('A' + chain_length - 10);
            } else if (chain_length > 35) {
                marker = '+';
            }

            putchar(marker);
            if ((i - start + 1) % 8 == 0 && i + 1 < end) {
                putchar(' ');
            }
        }
        putchar('\n');
    }
}