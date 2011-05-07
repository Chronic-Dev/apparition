#ifndef MBDB_H
#define MBDB_H

#include "mbdb_record.h"

#define MBDB_MAGIC "\x6d\x62\x64\x62\x05\x00"

typedef struct mbdb_property_t {
    char* name;
    char* value;
} mbdb_property_t;

typedef struct mbdb_header {
    unsigned char magic[6];		       // 'mbdb\5\0'
} mbdb_header_t;

typedef struct mbdb_t {
	mbdb_header_t* header;
    mbdb_record_t** records;
    mbdb_property_t** properties;
} mbdb_t;

extern mbdb_t* apparition_mbdb;

mbdb_t* mbdb_create();
mbdb_t* mbdb_open(unsigned char* file);
mbdb_t* mbdb_parse(unsigned char* data, unsigned int size);
void mbdb_free(mbdb_t* mbdb);

#endif

