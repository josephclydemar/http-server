#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>
#include <semaphore.h>
#include "queue.h"

#define WORKER_THREAD_COUNT      5

extern struct server_mgr {
  sem_t worker_thread_available;
  pthread_mutex_t t_queue_lock;
  pthread_cond_t t_queue_not_empty_cond;
  pthread_t worker_tid[WORKER_THREAD_COUNT];
  pthread_t server_tid;
  struct queue t_queue;
} server_mgr;

extern struct logger_mgr {
  pthread_mutex_t logger_lock;
  pthread_cond_t logger_queue_not_empty_cond;
  pthread_t logger_tid;
  struct queue logger_queue;
  char holder_buf[DTYPE_STR_MAX_LEN];
} logger_mgr;


void* worker_fn(void* param);
void* listener_fn(void* param);

#endif // SERVER_H_
