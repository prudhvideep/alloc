#include "alloc.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

bool is_power_of_two(size_t x){
  return (x & (x-1)) == 0;
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

size_t calc_padding_with_header(uintptr_t addr, uintptr_t alignment, size_t header_size) {
  uintptr_t p,a,modulo,padding = 0;
  uintptr_t required_space = (uintptr_t) header_size;

  assert(is_power_of_two(alignment));

  p = addr;
  a = alignment;

  modulo = p & (a-1); 
  if(modulo != 0) {
    padding = a - modulo;
  }
   
  if(padding < required_space) {
    required_space -= padding;

    if((required_space & (a-1)) != 0) {
      padding += a * (1 + (required_space/a));
    }else {
      padding += a * (required_space/a);
    }
  }

  return (size_t) padding;
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

void *arena_alloc(Arena *arena, size_t alloc_size) {
  return arena_alloc_aligned(arena, alloc_size, DEFAULT_ALIGNMENT);
}

void *arena_alloc_aligned(Arena *arena,size_t alloc_size, size_t alignment) {
  uintptr_t cur_addr = (uintptr_t) &arena->buf[arena->cur_offset];
  uintptr_t aligned_addr = align_forward(cur_addr, alignment);

  size_t align_offset = (aligned_addr - cur_addr);

  if((arena->cur_offset + align_offset + alloc_size) < arena->arena_capacity) {
    arena->cur_offset += align_offset + alloc_size;
    
    return (void *) aligned_addr;
  }
  
  return NULL;
}

void free_list_init(FreeList *list, size_t mem_size) {
  list->data = mmap(NULL, mem_size,
                    PROT_READ | PROT_WRITE, 
                    MAP_ANONYMOUS | MAP_PRIVATE, 
                    -1, 0);
  
  list->size = mem_size;

  FreeListNode *first_node = (FreeListNode *) list->data;
  first_node->block_size = mem_size;
  first_node->next = NULL;

  list->head = first_node;
}

void free_list_destroy(FreeList *list) {
  munmap(list->data, list->size);

  list->size = 0;
  list->head = NULL;
}

void* free_list_alloc(FreeList *list, size_t alloc_size) {
  return free_list_alloc_aligned(list, alloc_size, DEFAULT_ALIGNMENT);
}

void* free_list_alloc_aligned(FreeList *list, size_t alloc_size, size_t alignment) {
  FreeListNode *cur_node, *prev_node;
  size_t padding,required_size;
  size_t header_size = sizeof(FreeListHeader), node_size = sizeof(FreeListNode);

  cur_node = list->head;
  prev_node = NULL;
  
  // Traverse the free list identify node with enough capacity
  while (cur_node != NULL) {
    padding = calc_padding_with_header((uintptr_t) cur_node, alignment, sizeof(FreeListHeader));
    required_size = padding + alloc_size;

    if(cur_node->block_size >= required_size) {
      break;
    }
    
    prev_node = cur_node;
    cur_node = cur_node->next;
  }
  
  if(cur_node == NULL) {
    assert(0 && "could not find free list node with required capacity.");
    return NULL;
  }
  

  size_t cur_node_size = cur_node->block_size;
  FreeListNode *next_node = cur_node->next;
  
  uintptr_t cur_node_addr = (uintptr_t) cur_node;
  uintptr_t alloc_aligned_addr = cur_node_addr + (uintptr_t) padding;
  
  // Set the header metedata
  FreeListHeader *header = (FreeListHeader *)(alloc_aligned_addr - (uintptr_t) header_size);
  header->alloc_size = alloc_size;
  header->padding = padding;

  size_t remaining_block_size = cur_node_size - (padding + alloc_size);
  if(remaining_block_size >= node_size) {
    
    uintptr_t cur_node_resize_addr = (uintptr_t) cur_node + (uintptr_t) (padding + alloc_size);
    cur_node = (FreeListNode *) cur_node_resize_addr;
    cur_node->block_size = remaining_block_size;
    cur_node->next = next_node;

    
    if(prev_node) prev_node->next = cur_node;
    else list->head = cur_node;
  } else{
    if(prev_node) prev_node->next = next_node;
    else list->head = next_node;
  }

  return  (void *) alloc_aligned_addr;

}

void free_list_dealloc(FreeList *list, void *alloc_addr) {
  size_t padding, alloc_size, header_size = sizeof(FreeListHeader);
  FreeListHeader * header_ptr = (FreeListHeader *) ((uintptr_t) alloc_addr - (uintptr_t) header_size);

  assert(list != NULL);
  
  padding = header_ptr->padding;
  alloc_size = header_ptr->alloc_size;

  uintptr_t block_addr = (uintptr_t) alloc_addr - (uintptr_t) padding;
  size_t block_size = alloc_size + padding;
  
  // Reset the memory block
  memset((void *)block_addr, 0, block_size);
  
  // Add the block meta data
  FreeListNode *node = (FreeListNode *) block_addr;
  node->block_size = block_size;
  node->next = NULL;

  // Add the block to the free list
  FreeListNode * first_node = list->head;
  
  list->head = node;
  node->next = first_node;
}

