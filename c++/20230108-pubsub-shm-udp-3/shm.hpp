#pragma once

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <string>

#define ENTRY_NUM_MAX 64

#define AERROR std::cerr

// 共享状态
class State {
 public:
  explicit State(int block_num, int block_size)
      : block_num_(block_num), block_size_(block_size) {}

  int LockForWrite() {
    for (int i = write_pos_;; i = (i + 1) % block_num_) {
      int rw_lock_free = kRWLockFree;
      if (locks_[i].compare_exchange_weak(rw_lock_free, kWriteExclusive,
                                          std::memory_order_acq_rel,
                                          std::memory_order_relaxed)) {
        return i;
      }
    }

    // unreachable
    return -1;
  }
  void ReleaseWriteLock(int i) {
    // locks_[i].fetch_add(1);
    locks_[i].store(kRWLockFree);
    // read_pos_.store(i, std::memory_order_relaxed);
    read_pos_.store(i);
  }

  int LockForRead() {
    while (1) {  // TODO 未考虑重试次数，一直读，直到读到有效内容
      int i = read_pos_.load(
          std::memory_order_relaxed);  // 这里使用内存排序，提升明显
      if (i < 0) {
        // 未曾写入过
        AERROR << "read_pos_ < 0" << std::endl;
        return -1;
      }

      int lock_num = locks_[i].load();
      if (i < kRWLockFree) {
        // 正在写
        AERROR << "LockForRead read_pos_ < kRWLockFree" << std::endl;
        continue;
      }
      if (locks_[i].compare_exchange_weak(lock_num, lock_num + 1,
                                          std::memory_order_acq_rel,
                                          std::memory_order_relaxed)) {
        return i;
      }
    }

    // unreachable
    return -1;
  }
  void ReleaseReadLock(int i) { locks_[i].fetch_sub(1); }

 private:
  const static int kRWLockFree = 0;
  const static int kWriteExclusive = -1;

  std::atomic<int> block_num_;
  std::atomic<int> block_size_;

  // producer下次将要写入的位置。只有producer访问，当前是单生产者模式，无需原子
  int write_pos_{0};
  // producer最近写入成功的位置，即消费者需要读取的位置。是producer和consumer同步的关键
  std::atomic<int> read_pos_{-1};
  std::atomic<int> refcount_{0};
  std::atomic<int> locks_[ENTRY_NUM_MAX]{};
};

class ShmBlock {
 public:
  explicit ShmBlock(const std::string &name, int block_num, int block_size)
      : shm_name_(name),
        block_num_(block_num),
        block_size_(block_size),
        cap_(sizeof(State) + block_num * block_size) {}

  ~ShmBlock() {
    if (managed_shm_) shm_unlink(shm_name_.c_str());
  }

  void *AcquireBlockToWrite() {
    if (managed_shm_ == NULL && !Create()) {
      fprintf(stderr, "Create error");
      return NULL;
    }
    int i = state_->LockForWrite();
    // std::cout << "AcquireBlockToWrite LockForWrite: " << i
    //           << ", void*: " << (void *)(buffer_ + i * block_size_)
    //           << std::endl;
    return buffer_ + i * block_size_;
  }

  void ReleaseWrittenBlock(void *buf) {
    int i = ((char *)buf - buffer_) / block_size_;
    state_->ReleaseWriteLock(i);
  }

  void *AcquireBlockToRead() {
    if (managed_shm_ == NULL && !Open()) {
      AERROR << "failed to open shared memory, can't read now.";
      return NULL;
    }
    int i = state_->LockForRead();
    assert(i >= 0);
    return buffer_ + i * block_size_;
  }

  void ReleaseReadBlock(void *buf) {
    int i = ((char *)buf - buffer_) / block_size_;
    state_->ReleaseReadLock(i);
  }

  inline bool Create();
  inline bool Open();
  inline void Stop();

 private:
  std::string shm_name_;  // TODO 是不是应该改成char[]
  int block_num_;
  int block_size_;

  int cap_;
  void *managed_shm_{nullptr};  // 内存映射地址
  State *state_{nullptr};
  char *buffer_{NULL};
};

bool ShmBlock::Create() {
  if (managed_shm_ != NULL) {
    return true;
  }

  // create managed_shm_
  int fd = shm_open(shm_name_.c_str(), O_RDWR | O_CREAT, 0644);  //  | O_EXCL
  if (fd < 0) {
    if (EEXIST == errno) {
      AERROR << "shm already exist, open only.\n";
      //   return OpenOnly();
    } else {
      std::cerr << "create shm failed, error: " << strerror(errno)
                << std ::endl;
      return false;
    }
  }
  // TODO cap_应该时off_t类型，64位是long long int
  if (ftruncate(fd, cap_) < 0) {
    AERROR << "ftruncate failed: " << strerror(errno) << std::endl;
    close(fd);
    return false;
  }

  // attach managed_shm_
  managed_shm_ = mmap(nullptr, cap_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (managed_shm_ == MAP_FAILED) {
    AERROR << "attach shm failed:" << strerror(errno);
    close(fd);
    shm_unlink(shm_name_.c_str());
    return false;
  }

  close(fd);

  // create field state_
  state_ = new (managed_shm_) State(block_num_, block_size_);
  if (state_ == nullptr) {
    AERROR << "create state failed.";
    munmap(managed_shm_, cap_);
    managed_shm_ = nullptr;
    shm_unlink(shm_name_.c_str());
    return false;
  }

  buffer_ = (char *)managed_shm_ + sizeof(State);
  //   std::cout << "Create success\n";
  return true;
}

bool ShmBlock::Open() {
  if (managed_shm_ != nullptr) {
    return true;
  }

  // get managed_shm_
  int fd = shm_open(shm_name_.c_str(), O_RDWR, 0644);
  if (fd == -1) {
    // AERROR << "shm_open error: " << strerror(errno) << std::endl;
    return false;
  }

  struct stat file_attr;
  if (fstat(fd, &file_attr) < 0) {
    AERROR << "fstat failed: " << strerror(errno) << std::endl;
    close(fd);
    return false;
  }
  // attach managed_shm_
  managed_shm_ = mmap(nullptr, file_attr.st_size, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);
  if (managed_shm_ == MAP_FAILED) {
    AERROR << "OpenOnly attach shm failed: " << strerror(errno);
    std::cout << "size: " << file_attr.st_size << std::endl;
    close(fd);
    managed_shm_ = nullptr;
    return false;
  }

  close(fd);
  // get field state_
  state_ = reinterpret_cast<State *>(managed_shm_);
  if (state_ == nullptr) {
    AERROR << "get state failed.";
    munmap(managed_shm_, file_attr.st_size);
    managed_shm_ = nullptr;
    return false;
  }

  buffer_ = (char *)managed_shm_ + sizeof(State);
  //   std::cout << "Open success" << std::endl;
  return true;
}

void ShmBlock::Stop() {
  if (managed_shm_ != nullptr) {
    munmap(managed_shm_, cap_);
    managed_shm_ = nullptr;
  }
}
