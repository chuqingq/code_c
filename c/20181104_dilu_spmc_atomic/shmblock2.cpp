// g++ -g -lrt -o producer -DPRODUCER shmblock2.cpp -pthread -Wall; g++ -g -lrt -o consumer shmblock2.cpp -pthread -Wall
// ./producer &; ./consumer

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <atomic>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define ENTRY_NUM_MAX 64


// 共享状态
class State {
public:
    explicit State(int block_size, int block_num)
      : block_size_(block_size)
      , block_num_(block_num) {}
    int LockForWrite() {
      for (int i = write_pos;; i = (i + 1) % block_num_) {
          int rw_lock_free = kRWLockFree;
          if (locks[i].compare_exchange_weak(rw_lock_free,
                                             kWriteExclusive,
                                             std::memory_order_acq_rel,
                                             std::memory_order_relaxed)) {
              return i;
          }
      }

      // unreachable
      return -1;
    }
    void ReleaseWriteLock(int i) { locks[i].fetch_add(1); read_pos.store(i); }

    int LockForRead() {
        while (1) { // TODO 未考虑重试次数，一直读，直到读到有效内容
            int i = read_pos.load();
            if (i < 0) {
              continue; // 未曾写入过
            }

            int lock_num = locks[i].load();
            if (i < kRWLockFree) {
              continue;
            }
            if (locks[i].compare_exchange_weak(lock_num,
                                               lock_num+1,
                                               std::memory_order_acq_rel,
                                               std::memory_order_relaxed)) {
                return i;
            }
        }

        // unreachable
        return -1;
    }
    void ReleaseReadLock(int i) { locks[i].fetch_sub(1); }

    void IncreaseRefCount() { refcount.fetch_add(1); };
    int DecreaseRefCount() { return refcount.fetch_sub(1); }

    bool checkValid(int block_size, int block_num) const {
        return block_size_ == block_size && block_num_ == block_num;
    }
private:
    const static int kRWLockFree = 0;
    const static int kWriteExclusive = -1; 
    // State();
    int block_size_; // TODO 校验
    int block_num_;

    int write_pos{0}; // producer下次将要写入的位置。只有producer访问，当前是单生产者模式，无需原子
    std::atomic<int> read_pos{-1};  // producer最近写入成功的位置，即消费者需要读取的位置。是producer和consumer同步的关键

    std::atomic<int> refcount{0};
    // int nconsumers[ENTRY_NUM_MAX]; // 每个entry有多少个consumer在读，-1表示正在被producer写入。不用于同步，可以用relaxed
    // pthread_rwlock_t rwlocks[ENTRY_NUM_MAX];
    std::atomic<int> locks[ENTRY_NUM_MAX]{};
};

class ShmBlock {
public:
    explicit ShmBlock(const std::string &name, int block_num, int block_size)
        : shm_name_(name),
          block_num_(block_num),
          block_size_(block_size),
          cap_(sizeof(State) + block_num * block_size) {}// TODO reader需要和共享内存中匹配
    ~ShmBlock();

    void *AcquireBlockToWrite();
    void ReleaseWrittenBlock(void *block);

    void *AcquireBlockToRead();
    void ReleaseReadBlock(void* block);

private:
    bool OpenOrCreate();
    bool OpenOnly();

    std::string shm_name_;
    int block_num_; // entry_capacity
    int block_size_; // 每个entry的大小

    int cap_;     // 在使用的entry数量，只增不减，<=cap。目的是避免consumer读到从没写过的entry
    void *managed_shm_{nullptr}; // 内存映射地址
    State* state_{nullptr};
    char *buffer_{NULL};
};

// bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
// #define CAS(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)

// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
// #define LOAD(ptr) __sync_sub_and_fetch(ptr, 0)

#define AERROR std::cerr

bool ShmBlock::OpenOrCreate() {
  if (managed_shm_ != NULL) {
    return true;
  }

  // cap_ = sizeof(State) + block_num_ * block_size_;

  // create managed_shm_
  int fd = shm_open(shm_name_.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644);
  if (fd < 0) {
    if (EEXIST == errno) {
      AERROR << "shm already exist, open only.";
      return OpenOnly();
    } else {
      std::cerr << "create shm failed, error: " << strerror(errno);
      return false;
    }
  }


  if (ftruncate(fd, cap_) < 0) { // TODO cap_应该时off_t类型，64位是long long int
    AERROR << "ftruncate failed: " << strerror(errno);
    close(fd);
    return false;
  }


  // attach managed_shm_
  managed_shm_ = mmap(nullptr, cap_, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);
  if (managed_shm_ == MAP_FAILED) {
    AERROR << "attach shm failed:" << strerror(errno);
    close(fd);
    shm_unlink(shm_name_.c_str());
    return false;
  }

  close(fd);

  // create field state_
  state_ = new (managed_shm_) State(block_size_, block_num_);
  if (state_ == nullptr) {
    AERROR << "create state failed.";
    munmap(managed_shm_, cap_);
    managed_shm_ = nullptr;
    shm_unlink(shm_name_.c_str());
    return false;
  }

  state_->IncreaseRefCount();

  buffer_ = (char *)managed_shm_ + sizeof(State);
  std::cout << "OpenOrCreate success" << std::endl;
  return true;
}


bool ShmBlock::OpenOnly() {
  if (managed_shm_ != nullptr) {
    return true;
  }

  // get managed_shm_
  int fd = shm_open(shm_name_.c_str(), O_RDWR, 0644);
  if (fd == -1) {
    AERROR << "get shm failed: " << strerror(errno);
    return false;
  }

  struct stat file_attr;
  if (fstat(fd, &file_attr) < 0) {
    AERROR << "fstat failed: " << strerror(errno);
    close(fd);
    return false;
  }

  // attach managed_shm_
  managed_shm_ = mmap(nullptr, file_attr.st_size, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);
  if (managed_shm_ == MAP_FAILED) {
    AERROR << "attach shm failed: " << strerror(errno);
    close(fd);
    return false;
  }

  close(fd);
  // get field state_
  state_ = reinterpret_cast<State*>(managed_shm_);
  if (state_ == nullptr) {
    AERROR << "get state failed.";
    munmap(managed_shm_, file_attr.st_size);
    managed_shm_ = nullptr;
    return false;
  }

  // check block_size_ and block_num_
  if (!state->CheckValid(block_size_, block_num_)) {
    AERROR << "openonly check state invalid" << std::endl;
    // TODO 资源释放
    return false;
  }

  state_->IncreaseRefCount();
  buffer_ = (char *)managed_shm_ + sizeof(State);

  return true;
}


ShmBlock::~ShmBlock(){
    int rc = state_->DecreaseRefCount();
    if (rc == 1) {
        std::cout << "shmunlink(): " << shm_name_ << std::endl;
        if (shm_unlink(shm_name_.c_str()) < 0) {
            AERROR << "shm_unlink failed: " << strerror(errno);
        }
        // TODO 要搞成引用计数，确保最后一个释放的会删除文件
    }
}

void *ShmBlock::AcquireBlockToWrite() {
    // std::cout << "begin AcquireBlockToWrite" << std::endl;
    if (managed_shm_ == NULL && !OpenOrCreate()) {
        fprintf(stderr, "OpenOrCreate error");
        return NULL;
    }

    int i = state_->LockForWrite();
    return buffer_ + i * block_size_;
}

// 生产者写入后释放写锁
void ShmBlock::ReleaseWrittenBlock(void *buf)
{
    // 计算entry的位置
    int i = ((char *)buf - buffer_) / block_size_;
    state_->ReleaseWriteLock(i);
}

// 消费者

// 对申请的entry的buf只读
void *ShmBlock::AcquireBlockToRead()
{
    if (managed_shm_ == NULL && !OpenOnly()) {
        AERROR << "failed to open shared memory, can't read now.";
        return NULL;
    }

    int i = state_->LockForRead();
    return buffer_ + i * block_size_;
}

// 读取完成后解锁
void ShmBlock::ReleaseReadBlock(void *buf)
{
    int i = ((char *)buf - buffer_) / block_size_;
    state_->ReleaseReadLock(i);
}

/////////////////////////// 测试 ////////////////////////////////////////////////////////

#if 1

#include <sys/time.h>
#include <unistd.h>

static unsigned long long ustime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

ShmBlock *spmc;
const int count = 20000000;

const std::string kShmKey{"chuqq_shm"};

int main_p()
{
    printf("producer: \n");
    ShmBlock spmc(kShmKey, 4, 10);
    unsigned long long start = ustime();

    for (int i = 0; i < count; i++)
    {
        void *buf = spmc.AcquireBlockToWrite();
        if (buf == NULL)
        {
            printf("ERROR AcquireBlockToWrite NULL!!!\n");
            exit(-1); // 不应该返回NULL
        }

        // buf[0] = (uint8_t)i;
        *((int *)buf) = i;
        // usleep(70000);

        // printf("produce: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
        spmc.ReleaseWrittenBlock(buf);
        // printf("produce: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    }

    printf("producer: %llu\n", ustime() - start);
    sleep(3);
    return 0;
}

int main_c()
{
    printf("consumer: \n");
    ShmBlock spmc(kShmKey, 4, 10);
    // spmc = (spmc_t*)ShmInit(kShmKey, 4, 10, 1);
    unsigned long long start = ustime();

    int diff = 0; // 不同的次数
    int same = 0; // 相同的次数。这两个值用来判断consume的空转率
    uint8_t old = 1;
    for (;;)
    {
        void *buf = spmc.AcquireBlockToRead();
        if (buf == NULL)
        {
            printf("ERROR unexpected AcquireBlockToRead returns NULL\n");
            continue;
        }
        int value = *((int *)buf);

        if (old != ((uint8_t*)buf)[0])
        {
            // printf("consume diff: %d %p %d\n", value, &buf[0], i);
            old = ((uint8_t*)buf)[0];
            diff++;
        }
        else if (value == count - 1)
        {
            break;
        }
        else
        {
            // printf("consume: %d %p %d\n", value, &buf[0], i);
            same++;
        }

        // usleep(100000);
        // printf("consume: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
        spmc.ReleaseReadBlock(buf);
        // printf("consume: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    }

    printf("consumer: %llu, diff: %d, same: %d\n", ustime() - start, diff, same);

    return 0;
}

int main()
{
    printf("count: %d\n", count);
#ifdef PRODUCER
    return main_p();
#else
    return main_c();
#endif
}
#endif

/*
TODO
1、benchmark
    生产者生产COUNT个的时间
    消费者读取到COUNT的时间
2、加入校验逻辑：benchmark时可以不开
3、对比pthread_rw_lock.try_lock
4、封装成c++版本

# benchmark: 20000000 在dilu PC上大约1.5秒

*/
