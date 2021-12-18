#ifdef __linux__

#include <CHeap/abstraction.h>
#include <CHeap/config.h>
#include <CHeap/heap.h>

#include <sys/mman.h>
#include <strings.h>
#include <errno.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void* AllocPage(void* addr) {
    void* ptr = mmap(
        addr,
        CHEAP_CONFIG_PAGE_ALLOC_SIZE_KiB * 1024,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );
    
    if (ptr == MAP_FAILED) {
        if (errno == ENOMEM) {
            // Out of memory
            return NULL;
        }
        
        int error = errno;
        
        fprintf(stderr, "MMap error: ");

        switch (error)
        {
            case EACCES: fprintf(stderr, "EACCES"); break;
            case EAGAIN: fprintf(stderr, "EAGAIN"); break;
            case EBADF: fprintf(stderr, "EBADF"); break;
            case EINVAL: fprintf(stderr, "EINVAL"); break;
            case ENFILE: fprintf(stderr, "ENFILE"); break;
            case ENODEV: fprintf(stderr, "ENODEV"); break;
            case EPERM: fprintf(stderr, "EPERM"); break;
            case ETXTBSY: fprintf(stderr, "ETXTBSY"); break;
            case EOVERFLOW: fprintf(stderr, "EOVERFLOW"); break;
            default: fprintf(stderr, "Unknown %u", error); break;
        }

        fprintf(stderr, "\n");

        return FULL_PTR;
    }

    return ptr;
}

void MemoryZero(void* addr, uintptr_t size) {
    bzero(addr, size);
}

void FreePage(void* addr) {
    munmap(
        addr,
        CHEAP_CONFIG_PAGE_ALLOC_SIZE_KiB * 1024
    );
}

#ifdef __cplusplus
}
#endif

#endif