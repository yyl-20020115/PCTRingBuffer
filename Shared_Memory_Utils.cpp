#include "Shared_Memory_Utils.h"

void* shm_map2(const char* filename, size_t size, bool create)
{
    int flags = create ? (O_CREAT | O_RDWR) : O_RDWR;
    int fd = shm_open(filename, flags, 0666);
    if (fd == -1) {
        return nullptr;
    }
    if (create && 0!=ftruncate(fd, size)) {
        close(fd);
        return nullptr;
    }

    void* ret = (mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    close(fd);
    if (ret == MAP_FAILED) {
        return nullptr;
    }
    return ret;
}

