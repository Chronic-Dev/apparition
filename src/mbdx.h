#ifndef MBDX_H
#define MBDX_H

#define MBDX_MAGIC "\x6d\x62\x64\x78\x02\x00"

typedef struct mbdx_header_t {
    unsigned char magic[4];		  // "mbdx\2\0"
    unsigned int count;		  // count
} mbdx_header_t;

typedef struct mbdx_record_t {
    unsigned char key[20];		  // key / filename
    unsigned int offset;		  // offset of the mbdb file (+ 7 bytes)
    unsigned short mode;		  // filemode: Axxx = symlink, 4xxx = dir, 8xxx = file
} mbdx_record_t;

typedef struct mbdx_t {
	mbdx_header_t* header;
	mbdx_record_t** records;
} mbdx_t;

mbdx_t* mbdx_create();
void mbdx_free(mbdx_t* mbdx);

#endif

