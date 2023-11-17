// shared_memory_layout.h
#ifndef SHARED_MEMORY_LAYOUT_H
#define SHARED_MEMORY_LAYOUT_H

#include <atomic>
#include <cstdint>
#include <pthread.h>

struct alignas(64) SharedMemoryLayout {
    alignas(64) pthread_cond_t cond;
    alignas(64) pthread_mutex_t mutex;
    alignas(64) uint64_t messageId;
    alignas(64) uint64_t timestamp;
};

void initializeSharedMemory(SharedMemoryLayout* shared_data) ;

#endif
