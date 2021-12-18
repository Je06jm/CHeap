#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <CHeap/abstraction.h>
#include <CHeap/config.h>
#include <CHeap/heap.h>

#include <Windows.h>

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void* AllocPage() {
    void* ptr = VirtualAlloc(
        NULL,
        CHEAP_ALLOC_SIZE,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (ptr == NULL) {
        uint32_t err = GetLastError();
        
        switch (err) {
            case ERROR_NOT_ENOUGH_MEMORY:
            case ERROR_OUTOFMEMORY:
            case ERROR_COMMITMENT_LIMIT:
                // Out of memory
                return NULL;
            default:
                fprintf(stderr, "VirtualAlloc error: %u\n", err);
                return FULL_PTR;
        }
    }

    return ptr;
}

void MemoryZero(void* addr, uintptr_t size) {
    ZeroMemory(addr, size);
}

void FreePage(void* addr) {
    VirtualFree(
        addr,
        CHEAP_ALLOC_SIZE,
        MEM_DECOMMIT | MEM_RELEASE
    );
}

#ifdef __cplusplus
}
#endif

#endif