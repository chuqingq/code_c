#ifndef _BOUNDED_BLOCKING_QUEUE_H
#define _BOUNDED_BLOCKING_QUEUE_H

#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    size_t size;
    size_t head; // 可以get的位置
    size_t tail; // 可以put的位置
    int closed;
    void **data;
    pthread_mutex_t lock;
    pthread_cond_t readable;
    pthread_cond_t writeable;
} queue_t;

static int queue_init(queue_t *q, size_t size)
{
    if (q == NULL || size <= 1)
    {
        return -1;
    }

    q->size = size;
    q->head = 0;
    q->tail = 0;
    q->closed = 0;
    q->data = (void **)malloc(sizeof(*q->data) * size);
    if (q->data == NULL)
    {
        return -1;
    }

    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->readable, NULL);
    pthread_cond_init(&q->writeable, NULL);

    return 0;
}

static void queue_close(queue_t *q)
{
    pthread_mutex_lock(&q->lock);
    q->closed = 1;
    pthread_mutex_unlock(&q->lock);

    pthread_cond_broadcast(&q->readable);
    pthread_cond_broadcast(&q->writeable);
}

static void queue_destroy(queue_t *q)
{
    pthread_cond_destroy(&q->writeable);
    pthread_cond_destroy(&q->readable);
    pthread_mutex_destroy(&q->lock);

    if (q->data)
    {
        free(q->data);
    }
}

static int queue_put(queue_t *q, void *item)
{
    pthread_mutex_lock(&q->lock);
    while ((q->tail + 1) % q->size == q->head)
    {
        pthread_cond_wait(&q->writeable, &q->lock);
    }

    if (q->closed)
    {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }

    q->data[q->tail] = item;
    q->tail = (q->tail + 1) % q->size;

    pthread_cond_signal(&q->readable);
    pthread_mutex_unlock(&q->lock);
}

static void *queue_get(queue_t *q)
{
    pthread_mutex_lock(&q->lock);
    while (q->tail == q->head)
    {
        pthread_cond_wait(&q->readable, &q->lock);
    }

    if (q->closed)
    {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    void *ret = q->data[q->head];
    q->head = (q->head + 1) % q->size;

    pthread_cond_signal(&q->writeable);
    pthread_mutex_unlock(&q->lock);

    return ret;
}

#endif
