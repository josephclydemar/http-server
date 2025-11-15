#include "queue.h"
#include "debugging.h"
#include <stdlib.h>

struct qnode *create_qnode(union qnode_d item, enum dtype dtype) {
  size_t len = 0;
  struct qnode *new_node = (struct qnode *)malloc(sizeof(struct qnode));
  switch (dtype) {
    case Q_INT32:
      new_node->data.d_int32 = item.d_int32;
      break;
    case Q_FLOAT:
      new_node->data.d_float = item.d_float;
      break;
    case Q_STR:
      len = strlen(item.d_str);
      ASSERT("STRLEN", len < DTYPE_STR_MAX_LEN);
      memcpy(new_node->data.d_str, item.d_str, len + 1);
      break;
  }
  new_node->next = NULL;
  return new_node;
}

void init_queue(struct queue* queue, enum dtype dtype) {
  queue->front = NULL;
  queue->rear = NULL;
  queue->dtype = dtype;
  queue->count = 0;
}

struct queue *create_queue(enum dtype dtype) {
  struct queue *new_queue = (struct queue *)malloc(sizeof(struct queue));
  new_queue->front = NULL;
  new_queue->rear = NULL;
  new_queue->dtype = dtype;
  new_queue->count = 0;
  return new_queue;
}

void enqueue(struct queue *queue, union qnode_d item) {
  struct qnode *new_node = create_qnode(item, queue->dtype);
  if (queue->count == 0) {
    queue->rear = new_node;
    queue->front = queue->rear;
    queue->count++;
    return;
  }
  queue->rear->next = new_node;
  queue->rear = new_node;
  queue->count++;
}

/*
 * returns -1 if struct queue is empty
 * */
union qnode_d dequeue(struct queue *queue) {
  if (queue->count == 0) {
    return (union qnode_d)(-1);
  }
  struct qnode *node_to_remove = queue->front;
  union qnode_d item = node_to_remove->data;
  queue->front = node_to_remove->next;
  free(node_to_remove);
  queue->count--;
  return item;
}

/*
 * returns -1 if struct queue is empty
 * */
union qnode_d queue_peek(struct queue *queue) {
  if (queue->count == 0) {
    return (union qnode_d)(-1);
  }
  return queue->front->data;
}

