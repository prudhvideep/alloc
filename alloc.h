/* alloc.h */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <assert.h>
#include <sys/mman.h>


#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT 2 * (sizeof(void *))
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

typedef struct Arena Arena;
struct Arena {
    unsigned char * buf;
    size_t cur_offset;
    size_t arena_capacity; 
};

typedef struct FreeListHeader FreeListHeader;
struct FreeListHeader{
   size_t alloc_size;
   size_t padding;
};

typedef struct FreeListNode FreeListNode;
struct FreeListNode {
    size_t block_size;
    FreeListNode *next;
};

typedef struct FreeList FreeList;
struct FreeList {
    unsigned char * data;
    size_t size;
    
    FreeListNode *head;
};


void arena_init(Arena *arena, size_t mem_size);
void arena_destroy(Arena *arena);

void* arena_alloc(Arena *arena, size_t alloc_size);
void* arena_alloc_aligned(Arena *arena, size_t alloc_size, size_t alignment);

void free_list_init(FreeList *list, size_t mem_size);
void free_list_destroy(FreeList *list);

void* free_list_alloc(FreeList *list, size_t alloc_size);
void* free_list_alloc_aligned(FreeList *list, size_t alloc_size, size_t alignment);

void free_list_dealloc(FreeList *list, void *alloc_addr);
