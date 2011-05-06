#ifndef MBDB_H
#define MBDB_H

#include <unistd.h>
#include <time.h>

typedef struct mbdb_property_t {
    char* name;
    char* value;
} mbdb_property_t;

typedef struct mbdb_header {
    unsigned char magic[6];		       // 'mbdb\5\0'
} mbdb_header_t;

typedef struct mbdb_record_t {
    char* domain;
    char* path;
    char* target;	                  // absolute path
    char* datahash;	                  // SHA1 hash
    char* unknown1;
    unsigned short mode;	          // Axxx = symlink, 4xxx = dir, 8xxx = file
    unsigned int unknown2;
    unsigned int unknown3;
    unsigned int uid;
    unsigned int gid;
    unsigned int time1;
    unsigned int time2;
    unsigned int time3;
    unsigned long long length;	      // 0 if link or dir
    unsigned char flag;	              // 0 if link or dir
    unsigned char properties;	      // number of properties
} mbdb_record_t;

typedef struct mbdb_t {
	mbdb_header_t* header;
    mbdb_record_t** records;
    mbdb_property_t** properties;
} mbdb_t;

mbdb_t* mbdb_create();
void mbdb_free(mbdb_t* mbdb);

#endif

