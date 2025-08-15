/* alloc.h */

#include <stddef.h>

#ifndef DEFAULT_WORD
#define DEFAULT_WORD 2 * (sizeof(void *))
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

typedef struct {
    unsigned char * buf;
    size_t cur_offset;
    size_t arena_capacity; 
} Arena;

void arena_init(Arena *arena, size_t mem_size);

void* arena_alloc(Arena *arena, size_t allocation_size);
void* arena_alloc_aligned(Arena *arena, size_t allocation_size, size_t alignment);

void arena_destroy(Arena *arena);
