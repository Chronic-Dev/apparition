#ifndef MBDX_H
#define MBDX_H

#include <unistd.h>

struct mbdx_record {
    uint8_t key[20];		  // key / filename
    uint32_t offset;		  // offset of the mbdb file (+ 7 bytes)
    uint16_t mode;		  // filemode: Axxx = symlink, 4xxx = dir, 8xxx = file
    
    struct mbdx_record *next;
};

struct mbdx_file {
    uint8_t magic[4];		  // "mbdx\2\0"
    uint32_t count;		  // count
    struct mbdx_record *record;    
};

#endif

