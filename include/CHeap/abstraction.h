#ifndef CHEAP_ABSTRACTION_H
#define CHEAP_ABSTRACTION_H

#include <stdint.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

void* AllocPage(void* addr);
void MemoryZero(void* addr, uintptr_t size);
void FreePage(void* addr);

#ifdef __cplusplus
}
#endif

#endif