#include "shared_memory.h"

typedef struct {
  char *temp_buffer;
  size_t old_size;
} shm_copy_t;

void *func1(void *thread_args) {
  thread_args_t *args = (thread_args_t *)thread_args;

  args->shm->shm[args->tid] = args->input_str[args->tid];

  free(args);
  return NULL;
}

void *func2(void *thread_args) {
  thread_args_t *args = (thread_args_t *)thread_args;
  shared_mem_t *shm = args->shm;

  int tid = args->tid;
  int idx = rand() % args->shm_size;
  char *row = &shm->shm[idx * args->str_size];
  if (row[0] == 0) {
    snprintf(row, args->str_size, "thread %d", tid);
  } else {
    char temp[args->str_size];
    strcpy(temp, row);
    snprintf(row, args->str_size, "thread %d overwrote (%s)", tid, temp);
  }

  free(args);
  return NULL;
}

void test1(char *input_str, shm_copy_t *copy) {
  // basic arguments for shared memory and threads
  int num_threads = strlen(input_str);
  size_t shm_size = 1;
  size_t str_size = 1024;

  // init shared memory and run threads
  shared_mem_t *shm = shared_mem_init(shm_size, str_size);
  pthread_t *threads = thread_args_run(shm, func1, num_threads, input_str, shm_size, str_size);

  // wait for threads to finish and print results
  thread_args_clean(threads, num_threads);
  print_shm(*shm, shm_size, str_size);

  // copy shm data to temp buffer before cleaning up
  size_t old_size = shm_size * str_size;
  char *temp_buffer = malloc(old_size);
  memcpy(temp_buffer, shm->shm, old_size);
  copy->temp_buffer = temp_buffer;
  copy->old_size = old_size;

  // clean up
  shm_delete(shm);
}

void test2(char *input_str, shm_copy_t *copy) {
  // basic arguments for shared memory and threads
  size_t num_threads = 8;
  size_t shm_size = 5;
  size_t str_size = 1024;

  // init shared memory and run threads with copied data
  shared_mem_t *shm = shared_mem_init(shm_size, str_size);
  memcpy(shm->shm, copy->temp_buffer, copy->old_size);
  free(copy->temp_buffer);

  // run threads with new function
  pthread_t *threads = thread_args_run(shm, func2, num_threads, NULL, shm_size, str_size);

  // wait for threads to finish and print results
  thread_args_clean(threads, num_threads);
  print_shm(*shm, shm_size, str_size);

  // clean up
  shm_delete(shm);
}

int main(void) {
  srand(time(NULL));

  char *message_str = "yall r bums";
  char *input_str = malloc(sizeof(char) * (strlen(message_str) + 1));
  strcpy(input_str, message_str);

  shm_copy_t copy = {NULL, 0};

  test1(input_str, &copy);
  test2(input_str, &copy);

  free(input_str);

  return 0;
}
