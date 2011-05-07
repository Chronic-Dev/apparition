#ifndef MBDX_H
#define MBDX_H

#define MBDX_MAGIC "\x6d\x62\x64\x78\x02\x00"

struct mbdx_record_t;

struct mbdx_header_t {
    unsigned char magic[6];		  // "mbdx\2\0"
    unsigned int count;		  // count
} __attribute__((__packed__));

typedef struct mbdx_header_t mbdx_header_t;

typedef struct mbdx_t {
	struct mbdx_header_t* header;
	struct mbdx_record_t** records;
} mbdx_t;

mbdx_t* mbdx_create();
mbdx_t* mbdx_open(unsigned char* file);
mbdx_t* mbdx_parse(unsigned char* data, unsigned int size);
void mbdx_free(mbdx_t* mbdx);

#endif

//2276
//131071
