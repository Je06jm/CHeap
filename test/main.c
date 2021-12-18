#include <CHeap/heap.h>
#include <CHeap/abstraction.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define IS_OKAY() {\
    Heap_Error_t err = Heap_GetError();\
    if (err != HEAP_OK) {\
        printf("Heap error: %s\n", Heap_GetString(err));\
        exit(EXIT_FAILURE);\
    }}

uintptr_t ABS(uintptr_t a) {
    return a & (1 << (sizeof(uintptr_t) - 1)) ? -a : a;
}

int main() {
    uintptr_t size = CHEAP_ALLOC_SIZE;
    printf("Max heap size: %lu KiB\n",
        (uint64_t)CHEAP_CONFIG_MAX_HEAP_SIZE_KiB);
    printf("Heap page allocation size: %lu KiB\n",
        (uint64_t)CHEAP_CONFIG_PAGE_ALLOC_SIZE_KiB);
    printf("Minimum allocation size: %lu B\n",
        (uint64_t)CHEAP_CONFIG_MIN_ALLOC_SIZE);

    printf("Testing startup\n");

    Heap_Init();
    IS_OKAY();

    printf("Testing initial allocation\n");

    int* a = Heap_Alloc(sizeof(int));
    IS_OKAY();

    int* b = Heap_Alloc(sizeof(int));
    IS_OKAY();

    int* c = Heap_Alloc(sizeof(int));
    IS_OKAY();

    int* d = Heap_Alloc(sizeof(int));
    IS_OKAY();

    printf("Testing initial assignment\n");

    *a = 1;
    *b = 2;
    *c = 3;
    *d = 4;

    printf("Testing initial free\n");

    Heap_Free(a);
    IS_OKAY();
    Heap_Free(b);
    IS_OKAY();
    Heap_Free(d);
    IS_OKAY();
    Heap_Free(c);

    printf("Testing re-allocation\n");

    a = Heap_Alloc(sizeof(int));
    IS_OKAY();

    assert(*a == 1);

    b = Heap_Alloc(sizeof(int));
    IS_OKAY();

    assert(*b == 2);

    c = Heap_Alloc(sizeof(int));
    IS_OKAY();

    assert(*c == 3);

    printf("Testing zero allocation\n");

    d = Heap_ZAlloc(sizeof(int));
    assert(*d == 0);

    printf("Testing free of re-allocations\n");

    Heap_Free(a);
    IS_OKAY();
    Heap_Free(b);
    IS_OKAY();
    Heap_Free(c);
    IS_OKAY();
    Heap_Free(d);
    IS_OKAY();

    printf("Testing heap entry splitting\n");

    a = Heap_Alloc(sizeof(int) * 64);
    IS_OKAY();
    b = Heap_Alloc(sizeof(int));
    IS_OKAY();

    Heap_Free(a);
    IS_OKAY();

    a = Heap_Alloc(sizeof(int));
    IS_OKAY();
    c = Heap_Alloc(sizeof(int));
    IS_OKAY();

    uintptr_t b_ptr = (uintptr_t)b;
    uintptr_t c_ptr = (uintptr_t)c;

    assert(c_ptr < b_ptr);

    printf("Testing free of split heap entries\n");

    Heap_Free(a);
    IS_OKAY();
    Heap_Free(c);
    IS_OKAY();
    Heap_Free(b);
    IS_OKAY();

    printf("Testing large heap allocations\n");

    a = Heap_Alloc(size / 2);
    IS_OKAY();
    b = Heap_Alloc(size / 2);
    IS_OKAY();
    c = Heap_Alloc(size / 2);
    IS_OKAY();
    d = Heap_Alloc(size / 2);
    IS_OKAY();

    printf("Testing large allocation addresses\n");

    uintptr_t a_ptr = (uintptr_t)a;
    b_ptr = (uintptr_t)b;
    c_ptr = (uintptr_t)c;
    uintptr_t d_ptr = (uintptr_t)d;

    assert(ABS(a_ptr - b_ptr) >= CHEAP_ALLOC_SIZE);
    assert(ABS(b_ptr - c_ptr) >= CHEAP_ALLOC_SIZE);
    assert(ABS(c_ptr - d_ptr) >= CHEAP_ALLOC_SIZE);

    printf("Testing free of large allocations\n");

    Heap_Free(a);
    IS_OKAY();
    Heap_Free(c);
    IS_OKAY();
    Heap_Free(b);
    IS_OKAY();
    Heap_Free(d);
    IS_OKAY();

    printf("Testing re-allocation of large allocations\n");

    a = Heap_Alloc(size / 2);
    IS_OKAY();
    b = Heap_Alloc(size / 2);
    IS_OKAY();
    c = Heap_Alloc(size / 2);
    IS_OKAY();
    d = Heap_Alloc(size / 2);
    IS_OKAY();

    printf("Testing free of re-allocated large allocations\n");

    Heap_Free(a);
    IS_OKAY();
    Heap_Free(c);
    IS_OKAY();
    Heap_Free(b);
    IS_OKAY();
    Heap_Free(d);
    IS_OKAY();

    printf("Re-allocating large allocations for final test\n");

    a = Heap_Alloc(size / 2);
    IS_OKAY();
    b = Heap_Alloc(size / 2);
    IS_OKAY();
    c = Heap_Alloc(size / 2);
    IS_OKAY();
    d = Heap_Alloc(size / 2);
    IS_OKAY();

    printf("Testing heap finish with large allocations\n");

    Heap_Finish();

    printf("All tests complete\n");
    
    return 0;
}