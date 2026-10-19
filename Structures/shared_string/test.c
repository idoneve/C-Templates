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

#define STRING_SIZE 1024

typedef struct {
  char *shared_str;
  size_t length;
} shared_str_t;

void print_shared_str(shared_str_t str) {
  printf("\nlength: %lu\nstring: %s\n\n", str.length - 1, str.shared_str);
}

typedef struct {
  int tid;
  char *input_str;
  shared_str_t *shared_str;
  pthread_mutex_t *mutex;
} thread_args_t;

void *thread_func(void *arg) {
  thread_args_t *args = arg;

  args->shared_str->shared_str[args->tid] = args->input_str[args->tid];

  pthread_mutex_lock(args->mutex);
  ++args->shared_str->length;
  pthread_mutex_unlock(args->mutex);

  return NULL;
}

int main(void) {
  char *message_str = "i am";
  size_t thread_num = strlen(message_str) + 1;

  char *input_str = malloc(sizeof(char) * thread_num);
  strcpy(input_str, message_str);

  shared_str_t *shared_str = malloc(sizeof(shared_str_t));
  shared_str->shared_str = malloc(sizeof(char) * STRING_SIZE);
  shared_str->shared_str[thread_num] = '\0';
  shared_str->length = 0;

  pthread_t threads[thread_num];
  thread_args_t *thread_args[thread_num];

  pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  for (size_t i = 0; i < thread_num; ++i) {
    thread_args[i] = malloc(sizeof(thread_args_t));
    thread_args[i]->tid = i;
    thread_args[i]->input_str = input_str;
    thread_args[i]->shared_str = shared_str;
    thread_args[i]->mutex = mutex;

    if (pthread_create(&threads[i], NULL, thread_func, thread_args[i]) < 0) {
      perror("Error: running thread funcs");
      return 1;
    }
  }

  for (size_t i = 0; i < thread_num; ++i) {
    if (pthread_join(threads[i], NULL) < 0) {
      perror("Error joining threads");
      return 1;
    }
    free(thread_args[i]);
  }

  print_shared_str(*shared_str);

  pthread_mutex_destroy(mutex);
  free(input_str);
  free(shared_str->shared_str);
  free(shared_str);
  return 0;
}
