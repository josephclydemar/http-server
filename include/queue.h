#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>

#define DTYPE_STR_MAX_LEN             1024

enum dtype {
  Q_INT32,
  Q_FLOAT,
  Q_STR,
};

union qnode_d {
  int d_int32;
  float d_float;
  char d_str[DTYPE_STR_MAX_LEN];
};

struct qnode {
  union qnode_d data;
  struct qnode* next;
};

struct queue {
  struct qnode* front;
  struct qnode* rear;
  enum dtype dtype;
  size_t count;
};

struct queue* create_queue(enum dtype dtype);
void init_queue(struct queue* queue, enum dtype dtype);
void enqueue(struct queue* queue, union qnode_d item);
union qnode_d dequeue(struct queue* queue);
union qnode_d queue_peek(struct queue* queue);

#endif

