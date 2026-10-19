#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct traveler_t traveler_t;
typedef struct trav_args_t trav_args_t;

typedef void *(*DoFunc)(trav_args_t *args);
typedef void  (*NextFunc)(trav_args_t *args);
typedef int   (*EndFunc)(trav_args_t *args);

struct traveler_t {
    void *p1;
    void *p2;

    uint64_t p1_idx;
    uint64_t p2_idx;
};

struct trav_args_t {
    uint64_t i;
    uint64_t size;
    void *tape;

    traveler_t *trav;

    DoFunc do_action;
    NextFunc next_action;
    EndFunc end_condition;
};

trav_args_t *travel_init(const char *);
void travel_free(trav_args_t *);

trav_args_t *travel_init(const char *start_str) {
    uint64_t str_len = strlen(start_str);

    if (start_str == NULL) {
        return NULL;
    }

    traveler_t *trav = (traveler_t *)malloc(sizeof(traveler_t));
    if (trav == NULL) {
        return NULL;
    }

    trav->p1_idx = 0;
    trav->p2_idx = 0;

    char *str = (char *)malloc(str_len + 1);
    if (str == NULL) {
        free(trav);
        return NULL;
    }

    memcpy(str, start_str, str_len + 1);

    trav_args_t *args = (trav_args_t *)malloc(sizeof(trav_args_t));
    if (args == NULL) {
        free(str);
        free(trav);

        return NULL;
    }

    args->trav = trav;
    args->tape = str;
    args->size = str_len;
    args->i = 0;

    return args;
}

void travel_free(trav_args_t *args) {
    if (args == NULL) {
        return;
    }

    free(args->trav);
    free(args->tape);

    free(args);
}

void *travel_do(trav_args_t *);
void travel_next(trav_args_t *);

int travel_end(trav_args_t *);

void travel(trav_args_t *);