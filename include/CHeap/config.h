#ifndef CHEAP_CONFIG_H
#define CHEAP_CONFIG_H

// Configuration defines

// Default is 0x80000 (512 MiBs). Set to zero to disable this
#define CHEAP_CONFIG_MAX_HEAP_SIZE_KiB 0x80000

// Default is 16
#define CHEAP_CONFIG_MIN_ALLOC_SIZE 16

// Default is 64 KiBs. This effects how big each alloc can be
#define CHEAP_CONFIG_PAGE_ALLOC_SIZE_KiB 64

// Do not modify the any part of the file below this point

#define CHEAP_ALLOC_SIZE (CHEAP_CONFIG_PAGE_ALLOC_SIZE_KiB * 1024)

#endif