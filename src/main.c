#include <stdio.h>
#include "debugging.h"
#include "server.h"

#define DEFAULT_PORT                     "3490"
#define GREEN_COLOR         0x080601

int main(int argc, char** argv) {
  char* buf = NULL;
  pthread_attr_t attr;
  ASSERT("PTHREAD_ATTR_INIT", pthread_attr_init(&attr) == 0);

  /* Thread Log */
  init_queue(&logger_mgr.logger_queue, Q_STR);
  ASSERT("PTHREAD_MUTEX_INIT", pthread_mutex_init(&logger_mgr.logger_lock, NULL) == 0);
  ASSERT("PTHREAD_COND_INIT", pthread_cond_init(&logger_mgr.logger_queue_not_empty_cond, NULL) == 0);

  /* Server Mgr */
  init_queue(&server_mgr.t_queue, Q_INT32);
  ASSERT("SEM_INIT", sem_init(&server_mgr.worker_thread_available, 0, WORKER_THREAD_COUNT) == 0);
  ASSERT("PTHREAD_MUTEX_INIT", pthread_mutex_init(&server_mgr.t_queue_lock, NULL) == 0);
  ASSERT("PTHREAD_COND_INIT", pthread_cond_init(&server_mgr.t_queue_not_empty_cond, NULL) == 0);

  for (size_t i = 0; i < WORKER_THREAD_COUNT; ++i) {
    ASSERT("PTHREAD_CREATE", pthread_create(&server_mgr.worker_tid[i], &attr, worker_fn, NULL) == 0);
  }
  ASSERT("PTHREAD_CREATE", pthread_create(&server_mgr.server_tid, &attr, listener_fn, argc > 1 ? argv[1] : DEFAULT_PORT) == 0);

  while (true) {
    pthread_mutex_lock(&logger_mgr.logger_lock);
    while (logger_mgr.logger_queue.count <= 0) {
      pthread_cond_wait(&logger_mgr.logger_queue_not_empty_cond, &logger_mgr.logger_lock);
    }
    buf = dequeue(&logger_mgr.logger_queue).d_str;
    pthread_mutex_unlock(&logger_mgr.logger_lock);
    printf("%s", buf);
  }
  return 0;
}

