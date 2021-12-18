#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <CHeap/abstraction.h>
#include <CHeap/config.h>

#include <Windows.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void* AllocPage(void* addr) {
    return VirtualAlloc(
        addr,
        CHEAP_ALLOC_SIZE,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
        PAGE_READWRITE
    );
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