bits 64
global memspace
%define Heapsize (1024*1024*1024/4)

Section .data alloc noexec write
    memspace:
        dd _memspace

Section .heap alloc noexec write nobits
    _memspace:
        heapsize equ Heapsize
        resd heapsize

Section .note.GNU-stack noalloc