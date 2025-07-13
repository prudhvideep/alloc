/* alloc.h */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define packed __attribute__((packed))
#define unused __attribute__((unused))
#define Maxwords ((1024 * 1024 * 1024 / 4) -1)

#define ErrNoMem 1

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;
typedef _BitInt(128) int128;
typedef void heap;
typedef int32 word;

struct packed s_header{
    word w:30;
    bool allocated:1;
    bool reserved:1;
};

typedef struct packed s_header header;

#define $1 (int8)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $16 (int128)
#define $c (char *)
#define $i (int)
#define $v (void *)
#define $h (header *)

#define reterr(x) errno = (x); return $v 0;
void * alloc(int32);