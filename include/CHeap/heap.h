#ifndef CHEAP_HEAP_H
#define CHEAP_HEAP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Heap_Error_e {
    HEAP_OK,
    HEAP_ERROR_NOT_INIT,
    HEAP_ERROR_IS_INIT,
    HEAP_ERROR_NO_MEMORY,
    HEAP_ERROR_MAX_MEMORY,
    HEAP_ERROR_ALLOC_TO_BIG,
    HEAP_ERROR_CANNOT_FREE,
    HEAP_ERROR_PLATFORM
} Heap_Error_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef FULL_PTR
#define FULL_PTR ((void*)(-1))
#endif

bool Heap_Init();
void* Heap_Alloc(uintptr_t size);
void* Heap_ZAlloc(uintptr_t size);
bool Heap_Free(void* pointer);
void Heap_Finish();

Heap_Error_t Heap_GetError();
const char* Heap_GetString(Heap_Error_t err);

#ifdef __cplusplus
}
#endif

#endif