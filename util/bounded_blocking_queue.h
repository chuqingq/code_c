#ifndef _BOUNDED_BLOCKING_QUEUE_H
#define _BOUNDED_BLOCKING_QUEUE_H

#include <pthread.h>
#include <stdlib.h>

typedef struct {
  size_t size;
  size_t out;  // 可以get的位置
  size_t in;   // 可以put的位置
  int closed;
  void **data;
  pthread_mutex_t lock;
  pthread_cond_t not_empty;  // 变为不空
  pthread_cond_t not_full;   // 变为不满
} queue_t;

static inline int queue_init(queue_t *q, size_t size) {
  if (q == NULL || size <= 0) {
    return -1;
  }

  q->size = size + 1;  // 多留一个空位用于区分full和empty
  q->out = 0;
  q->in = 0;
  q->closed = 0;
  q->data = (void **)malloc(sizeof(*q->data) * q->size);
  if (q->data == NULL) {
    return -1;
  }

  pthread_mutex_init(&q->lock, NULL);
  pthread_cond_init(&q->not_empty, NULL);
  pthread_cond_init(&q->not_full, NULL);

  return 0;
}

static inline void queue_close(queue_t *q) {
  pthread_mutex_lock(&q->lock);
  q->closed = 1;
  pthread_mutex_unlock(&q->lock);

  pthread_cond_broadcast(&q->not_empty);
  pthread_cond_broadcast(&q->not_full);
}

static inline void queue_destroy(queue_t *q) {
  pthread_cond_destroy(&q->not_full);
  pthread_cond_destroy(&q->not_empty);
  pthread_mutex_destroy(&q->lock);

  if (q->data) {
    free(q->data);
  }
}

static inline int queue_put(queue_t *q, void *item) {
  pthread_mutex_lock(&q->lock);
  while (1) {
    if (q->closed) {
      pthread_mutex_unlock(&q->lock);
      return -1;
    }
    if ((q->in + 1) % q->size != q->out) {
      break;
    }
    pthread_cond_wait(&q->not_full, &q->lock);
  }

  q->data[q->in] = item;
  q->in = (q->in + 1) % q->size;

  pthread_cond_signal(&q->not_empty);
  pthread_mutex_unlock(&q->lock);
  return 0;
}

static inline void *queue_get(queue_t *q) {
  pthread_mutex_lock(&q->lock);
  while (1) {
    if (q->closed) {
      pthread_mutex_unlock(&q->lock);
      return NULL;
    }
    if (q->in != q->out) {
      break;
    }
    pthread_cond_wait(&q->not_empty, &q->lock);
  }

  void *ret = q->data[q->out];
  q->out = (q->out + 1) % q->size;

  pthread_cond_signal(&q->not_full);
  pthread_mutex_unlock(&q->lock);

  return ret;
}

#endif
