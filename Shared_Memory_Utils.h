#ifndef SHARED_MEMORY_UTILS_H
#define SHARED_MEMORY_UTILS_H

#include <fcntl.h>      // For O_* constants
#include <sys/mman.h>   // For mmap
#include <unistd.h>     // For ftruncate, close
#include <sys/stat.h>   // For mode constants
#include <iostream>
#include <cstring>      // For strerror

void* shm_map2(const char* filename, size_t size, bool create = false);

template<class T>
T* shm_map(const char* filename,size_t ext_size, bool create = false) {
    int flags = create ? (O_CREAT | O_RDWR) : O_RDWR;
    int fd = shm_open(filename, flags, 0666);
    if (fd == -1) {
        return nullptr;
    }
    if (create && 0!=ftruncate(fd, sizeof(T) + ext_size)) {
        close(fd);
        return nullptr;
    }
    T* ret = static_cast<T*>(mmap(nullptr, sizeof(T) + ext_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    close(fd);
    if (ret == MAP_FAILED) {
        return nullptr;
    }
    return ret;
}

template<class T>
void shm_unmap(T* ptr) {
    if(ptr!=nullptr){
        munmap(ptr, sizeof(T));
    }
}

template<class T>
void shm_delete(const char *filename, T* ptr) {
    if(ptr!=nullptr){
        shmunmap(ptr);
    }
    if(filename!=nullptr){
        shm_unlink(filename);
    }
}


#endif // SHARED_MEMORY_UTILS_H
