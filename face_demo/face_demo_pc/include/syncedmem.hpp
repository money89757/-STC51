#ifndef CAFFE_SYNCEDMEM_HPP_
#define CAFFE_SYNCEDMEM_HPP_

#include <cstdlib>


// If CUDA is available and in GPU mode, host memory will be allocated pinned,
// using cudaMallocHost. It avoids dynamic pinning for transfers (DMA).
// The improvement in performance seems negligible in the single GPU case,
// but might be more significant for parallel training. Most importantly,
// it improved stability for large models on many GPUs.
inline void CaffeMallocHost(void** ptr, size_t size) {

  *ptr = malloc(size);

}

inline void CaffeFreeHost(void* ptr) {

  free(ptr);
}


/**
 * @brief Manages memory allocation and synchronization between the host (CPU)
 *        and device (GPU).
 *
 * TODO(dox): more thorough description.
 */
class SyncedMemory {
 public:
  SyncedMemory()
      : cpu_ptr_(NULL),  size_(0), head_(UNINITIALIZED),
        own_cpu_data_(false){}
  explicit SyncedMemory(size_t size)
      : cpu_ptr_(NULL),  size_(size), head_(UNINITIALIZED),
        own_cpu_data_(false){}
  ~SyncedMemory();
  const void* cpu_data();
  void set_cpu_data(void* data);

  void* mutable_cpu_data();
  enum SyncedHead { UNINITIALIZED, HEAD_AT_CPU, SYNCED };
  SyncedHead head() { return head_; }
  size_t size() { return size_; }


 private:
  void to_cpu();
  void* cpu_ptr_;

  size_t size_;
  SyncedHead head_;
  bool own_cpu_data_;

};  // class SyncedMemory



#endif  // CAFFE_SYNCEDMEM_HPP_
