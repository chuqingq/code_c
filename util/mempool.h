// TODO

#ifndef __MPP_MEM_POOL_H__
#define __MPP_MEM_POOL_H__

#include <list>
#include <mutex>

typedef struct {
  int max_block_num;
  int block_size;
  std::mutex mutex;
  std::list<char *> blocks;
} mempool_t;

mempool_t *mempool_init(mempool_t *pool, int max_block_num, int block_size);
void mempool_destroy(mempool_t *pool);

char *mempool_alloc(mempool_t *pool);
void mempool_release(mempool_t *buffer, char *block);

#endif