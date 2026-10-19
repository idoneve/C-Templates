#include "shared_memory.h"

shared_mem_t *shared_mem_init(size_t shm_size, size_t str_size) {
  shared_mem_t *shm = malloc(sizeof(shared_mem_t));
  if (!shm) {
    perror("Error: malloc shm");
    exit(1);
  }

  shm->shmid = shmget(IPC_PRIVATE, shm_size * str_size, IPC_CREAT | 0666);
  if (shm->shmid < 0) {
    perror("Error: shmget");
    free(shm);
    exit(1);
  }

  shm->shm = (char *)shmat(shm->shmid, NULL, 0);
  if (shm->shm == (char *)-1) {
    perror("Error: shmat");
    shmctl(shm->shmid, IPC_RMID, NULL);
    free(shm);
    exit(1);
  }

  shm->mutex = malloc(sizeof(pthread_mutex_t));
  if (!shm->mutex) {
    perror("Error: malloc mutex");
    shmdt(shm->shm);
    shmctl(shm->shmid, IPC_RMID, NULL);
    free(shm);
    exit(1);
  }
  pthread_mutex_init(shm->mutex, NULL);

  memset(shm->shm, 0, shm_size * str_size);

  return shm;
}

void print_shm(shared_mem_t shm, size_t shm_size, size_t str_size) {
  printf("\nshmid: %d\n", shm.shmid);
  for (size_t i = 0; i < shm_size; ++i) {
    char *row = &shm.shm[i * str_size];
    if (row[0] != 0) {
      printf("| %zu: %s |\n", i, row);
    } else {
      printf("| %zu |\n", i);
    }
  }
  printf("\n");
}

void shm_delete(shared_mem_t *shm) {
  shmdt(shm->shm);
  shmctl(shm->shmid, IPC_RMID, NULL);
  pthread_mutex_destroy(shm->mutex);
  free(shm->mutex);
  free(shm);
}

thread_args_t *thread_args_init(shared_mem_t *shm, int tid, char *input_str,
                                size_t shm_size, size_t str_size) {
  thread_args_t *args = malloc(sizeof(thread_args_t));
  if (!args) {
    perror("Error: malloc thread_args");
    exit(1);
  }

  args->shm = shm;
  args->tid = tid;
  args->input_str = input_str;
  args->shm_size = shm_size;
  args->str_size = str_size;

  return args;
}

pthread_t *thread_args_run(shared_mem_t *shm, void *func(void *),
                           int num_threads, char *input_str, size_t shm_size,
                           size_t str_size) {
  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
  for (int i = 0; i < num_threads; ++i) {
    thread_args_t *args =
        thread_args_init(shm, i, input_str, shm_size, str_size);
    if (pthread_create(&threads[i], NULL, func, args) < 0) {
      perror("Error: pthread_create");
      free(args);
      exit(1);
    }
  }
  return threads;
}

void thread_args_clean(pthread_t *threads, int num_threads) {
  for (int i = 0; i < num_threads; ++i) {
    if (pthread_join(threads[i], NULL) < 0) {
      perror("Error: pthread_join");
      free(threads);
      exit(1);
    }
  }
  free(threads);
}
