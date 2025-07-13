#include "alloc.h"

extern heap *memspace;

void *alloc(int32 bytes) {
    word words;
    header *hdr;
    void *mem;
    int32 ret;

    words = (!(bytes % 4)) ?
            bytes/4 : 
            (bytes/4) +1;

    mem = $v memspace;
    hdr = $h mem;

    (!(hdr->w)) ? ({
        if(words > Maxwords) 
            reterr(ErrNoMem)
        
    }) : ({
        
    });
}

int main(int argc, char **argv) {
    alloc(7);

    return 0;
}