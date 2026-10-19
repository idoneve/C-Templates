#include "travel.h"

void *travel_do(trav_args_t *args) {
    /* Do something */
    printf(
        "i: %lu\np1: %c, p1_idx: %lu\np2: %c, p2_idx: %lu\nstr: %s\n\n",
        args->i,
        *(char *)args->trav->p1, args->trav->p1_idx,
        *(char *)args->trav->p2, args->trav->p2_idx,
        (char *)args->tape
    ); // print all internal variables

    return NULL; // Deterministic BLEH
}

void travel_next(trav_args_t *args) {
    /* Iterate struct index */
    ++args->i;

    /* Move pointers */
    args->trav->p1 = args->trav->p2;
    args->trav->p2 = (char *)args->tape + args->i;

    /* Update pointer index */
    args->trav->p1_idx = args->trav->p2_idx;
    args->trav->p2_idx = args->i;
}

int travel_end(trav_args_t *args) {
    /* Return True until end condition is met then return False */
    if (args->i < args->size) { // End condition
        return 1; // Keep running
    }

    return 0; // Stop running
}

void travel(trav_args_t *args) {
    /* Initialize pointers and struct index */
    args->trav->p1 = (void *)args->tape;
    args->trav->p2 = (void *)args->tape;
    args->i = 0;

    /* While end condition is not met: do then iterate */
    while (travel_end(args)) {
        /* Do */
        void **ret = travel_do(args);
        if (ret != NULL) {
            // NON-DETERMINISM!!
        }

        /* Set up next iteration */
        travel_next(args);
    }
}

int main(void) {
    const char *start_str = "hello";
    trav_args_t *args = travel_init(start_str);
    if (args == NULL) {
        return 1;
    }

    travel(args);

    travel_free(args);

    return 0;
}
