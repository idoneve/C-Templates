#include "shared_memory.h"

void *func(void *t_args) {
    thread_args_t *args = (thread_args_t *)t_args;
    int tid = args->tid;
    char *input_str = args->input_str;
    size_t str_size = args->str_size;
    shared_mem_t *shm = args->shm;
    size_t shm_size = args->shm_size;

    for (int i = 0; 0 < )

    return NULL;
}

int main(void) {
    int num_threads = 8;
    int shm_size = 32;
    int str_size = 128;

    char *message_str = "uGH wWHateVer MaJOR losEr bItCH!!!! fjfuioewjeiowfjew83u1y97248yru9fiewjk?><./,/,/";
    char *input_str = malloc(sizeof(char) * (strlen(message_str) + 1));
    strcpy(input_str, message_str);

    shared_mem_t *shm = shared_mem_init(shm_size, str_size);
    
    pthread_t *threads = thread_args_run(shm, func, num_threads, input_str, shm_size, str_size);
    thread_args_clean(threads, num_threads);

    print_shm(*shm, shm_size, str_size);

    shm_delete(shm);
    return 0;
}