#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  char *shm;
  int shmid;
  pthread_mutex_t *mutex;
} shared_mem_t;

typedef struct {
  int tid;
  char *input_str;
  shared_mem_t *shm;
  size_t shm_size;
  size_t str_size;
} thread_args_t;

shared_mem_t *shared_mem_init(size_t shm_size, size_t str_size);
void print_shm(shared_mem_t shm, size_t shm_size, size_t str_size);
void shm_delete(shared_mem_t *shm);

thread_args_t *thread_args_init(shared_mem_t *shm, int tid, char *input_str,
                                size_t shm_size, size_t str_size);
pthread_t *thread_args_run(shared_mem_t *shm, void *func(void *),
                           int num_threads, char *input_str, size_t shm_size,
                           size_t str_size);
void thread_args_clean(pthread_t *threads, int num_threads);

#endif
