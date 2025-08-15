#include "alloc.h"
#include <assert.h>
#include <stdio.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

typedef struct {
  double x;
  double y;
} Point;

bool is_power_of_two(size_t x){
  return (x & (x-1)) == 0;
}

void arena_init(Arena *arena, size_t mem_size) {
  arena->buf = mmap(NULL, mem_size,
                    PROT_READ | PROT_WRITE, 
                    MAP_ANONYMOUS | MAP_PRIVATE, 
                    -1, 0);

  arena->arena_capacity = mem_size;
  arena->cur_offset = 0;
}

void arena_destroy(Arena *arena) {
  if(arena->buf && arena->arena_capacity)
    munmap(arena->buf, arena->arena_capacity);
}

void *arena_alloc(Arena *arena, size_t allocation_size) {
  return arena_alloc_aligned(arena, allocation_size, DEFAULT_WORD);
}

uintptr_t align_forward(uintptr_t address, size_t alignment) {
  uintptr_t p,a,modulo;
  assert(is_power_of_two(alignment)); 
  
  p = address;
  a = (uintptr_t) alignment;

  modulo = p & (a-1);

  if(modulo != 0) {
    p += (a - modulo);
  }

  return p;
}

void *arena_alloc_aligned(Arena *arena,size_t allocation_size, size_t alignment) {
  uintptr_t cur_addr = (uintptr_t) &arena->buf[arena->cur_offset];
  uintptr_t aligned_addr = align_forward(cur_addr, alignment);

  size_t align_offset = (aligned_addr - cur_addr);

  if((arena->cur_offset + align_offset + allocation_size) < arena->arena_capacity) {
    arena->cur_offset += align_offset + allocation_size;
    
    return (void *) aligned_addr;
  }
  
  return NULL;
}