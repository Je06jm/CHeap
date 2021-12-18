#include <CHeap/heap.h>
#include <CHeap/abstraction.h>

#include <assert.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HeapEntry_s {
    struct HeapEntry_s* next;
    struct HeapEntry_s* last;
    uintptr_t size;
    bool used;
} HeapEntry_t;

typedef struct HeapPage_s {
    struct HeapPage_s* next;
    uint32_t allocations;
    HeapEntry_t entries[];
} HeapPage_t;

#define ENTRY_TO_DATA(entry)\
    ((void*)(((uintptr_t)entry)+sizeof(HeapEntry_t)))

#define DATA_TO_ENTRY(data)\
    ((void*)(((uintptr_t)data)-sizeof(HeapEntry_t)))

Heap_Error_t heap_error = HEAP_OK;

HeapPage_t* first_page = NULL;
HeapPage_t* free_page = NULL;
uintptr_t pages_allocated = 0;

bool Heap_Init() {
    if (first_page != NULL) {
        heap_error = HEAP_ERROR_IS_INIT;
        return false;
    }

    first_page = AllocPage();

    if (first_page == NULL) {
        heap_error = HEAP_ERROR_NO_MEMORY;
        return false;
    } else if (first_page == FULL_PTR) {
        heap_error = HEAP_ERROR_PLATFORM;
        return false;
    }

    MemoryZero(first_page, sizeof(HeapPage_t));
    pages_allocated++;
    
    return true;
}

void* Heap_Alloc(uintptr_t size) {
    if (first_page == NULL) {
        heap_error = HEAP_ERROR_NOT_INIT;
        return NULL;
    }

    static const uintptr_t max_size = 
        CHEAP_ALLOC_SIZE - sizeof(HeapPage_t);
    
    if (size > max_size) {
        heap_error = HEAP_ERROR_ALLOC_TO_BIG;
        return NULL;
    }
    
    size = size < CHEAP_CONFIG_MIN_ALLOC_SIZE ?
        CHEAP_CONFIG_MIN_ALLOC_SIZE :
        size;

    HeapPage_t* page = first_page;

find_next_free_entry:;

    HeapEntry_t* current = &page->entries[0];
    while (current->next != NULL) {
        if ((current->size >= size) && !current->used) {
            goto alloc_data;
        }

        current = current->next;
    }

    // The current entry does not have a next. Either we reached the
    // end of the linked list or the page

    uintptr_t page_end = (uintptr_t)page;
    page_end += CHEAP_ALLOC_SIZE;

    uintptr_t next_entry = (uintptr_t)current;
    next_entry += sizeof(HeapEntry_t) + size;
    if (next_entry > page_end) {
        // Is there a page following this?
        if (page->next != NULL) {
            // There is so search it for a free entry
            page = page->next;
            goto find_next_free_entry;
        }

        // No size in this page
        HeapPage_t* new_page;

        // Is there a free page already allocated?
        if (free_page != NULL) {
            new_page = free_page;
            free_page = NULL;
            
        } else {
            // No. Allocate a new page

            static const uintptr_t max_pages = 
                (CHEAP_CONFIG_MAX_HEAP_SIZE_KiB  * 1024) /
                CHEAP_CONFIG_PAGE_ALLOC_SIZE_KiB;
                
            if ((max_pages != 0) && (pages_allocated >= max_pages)) {
                heap_error = HEAP_ERROR_MAX_MEMORY;
                return NULL;
            }

            new_page = AllocPage();

            if (new_page == NULL) {
                heap_error = HEAP_ERROR_NO_MEMORY;
                return NULL;
            } else if (new_page == FULL_PTR) {
                heap_error = HEAP_ERROR_PLATFORM;
                return NULL;
            }
        
            MemoryZero(new_page, sizeof(HeapPage_t));
            pages_allocated++;
        }

        page->next = new_page;
        page = new_page;

        current = &page->entries[0];
        next_entry = (uintptr_t)current;
        next_entry += sizeof(HeapEntry_t) + size;
    }

    HeapEntry_t* next = (HeapEntry_t*)next_entry;
    current->next = next;
    MemoryZero(next, sizeof(HeapEntry_t));
    next->last = current;


alloc_data:;
    // How big a free entry must be in order to be split into two.
    static const uintptr_t min_split_size =
        ((CHEAP_CONFIG_MIN_ALLOC_SIZE) << 1) + sizeof(HeapEntry_t);

    if (current->size >= min_split_size) {
        // Split the current entry into two
        next_entry = (uintptr_t)current;
        next_entry += sizeof(HeapEntry_t) + size;

        next = (HeapEntry_t*)next_entry;
        next->next = current->next;
        next->last = current;
        next->size = current->size - size - sizeof(HeapEntry_t);
        next->used = false;
        
        if (current->next->next != NULL) {
            current->next->next->last = next;
        }

        current->next = next;
    }
    
    page->allocations += size;
    current->size = size;
    current->used = true;
    
    return ENTRY_TO_DATA(current);
}

void* Heap_ZAlloc(uintptr_t size) {
    void* data = Heap_Alloc(size);

    if (data != NULL) {
        MemoryZero(data, size);
    }
    
    return data;
}

bool Heap_Free(void* pointer) {
    // We always return true on free NULL
    if (pointer == NULL) {
        return true;
    }

    // We need to check if we actually own the pointer
    uintptr_t address = (uintptr_t)pointer;
    uintptr_t page_start, page_end;
    HeapPage_t* page = first_page;
    HeapPage_t* last_page = NULL;

    while (page != NULL) {
        page_start = (uintptr_t)page;
        page_end = page_start + CHEAP_ALLOC_SIZE;

        if ((address >= page_start) && (address <= page_end)) {
            goto free_data;
        }

        // Not owned by this page
        last_page = page;
        page = page->next;
    }

    // The pointer is not owned by use
    heap_error = HEAP_ERROR_CANNOT_FREE;
    return false;

free_data:;
    // The data is owned by this page

    HeapEntry_t* entry = DATA_TO_ENTRY(pointer);
    entry->used = false;

    // Merge with free neighbors
    if ((entry->next != NULL) && !entry->next->used) {
        if (entry->next->next != NULL) {
            entry->next->next->last = entry;
        }
        
        entry->next = entry->next->next;
    }

    if ((entry->last != NULL) && !entry->last->used) {
        if (entry->next != NULL) {
            entry->next->last = entry->last;
        }
        
        entry->last->next = entry->next;
    }

    page->allocations -= entry->size;
    
    if (entry->next == NULL) {
        entry->size = 0;
    }

    // Does this page hold any data and is not the first page?
    if ((last_page != NULL) && (page->allocations == 0)) {
        // If not, mark it as a free page

        if (free_page) {
            // There is a free page already. We must free it first
            FreePage(free_page);
            pages_allocated--;
        }
        
        free_page = page;
        last_page->next = page->next;
    }

    return true;
}

void Heap_Finish() {
    // Release all pages
    HeapPage_t* page = first_page;
    HeapPage_t* next;
    
    while (page != NULL) {
        next = page->next;

        FreePage(page);

        page = next;
    }
}

Heap_Error_t Heap_GetError() {
    Heap_Error_t err = heap_error;
    heap_error = HEAP_OK;
    return err;
}

const char* Heap_GetString(Heap_Error_t err) {
    switch(err) {
        case HEAP_OK: return "OK";
        case HEAP_ERROR_NOT_INIT: return "Not initialized";
        case HEAP_ERROR_IS_INIT: return "Already initialized";
        case HEAP_ERROR_NO_MEMORY: return "Out of memory";
        case HEAP_ERROR_MAX_MEMORY: return "Reached the maximum memory allowed by the configuration";
        case HEAP_ERROR_ALLOC_TO_BIG: return "Allocation is too big";
        case HEAP_ERROR_CANNOT_FREE: return "Memory is not owned by this heap";
#ifdef __linux__
        case HEAP_ERROR_PLATFORM: return "Linux error. See cerr for more info";
#else
        case HEAP_ERROR_PLATFORM: return "Windows error. See cerr for more info";
#endif
        default: return "Invalid error code";
    }
}

#ifdef __cplusplus
}
#endif
