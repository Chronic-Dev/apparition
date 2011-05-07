#ifndef mbdx_record_H
#define mbdx_record_H

typedef struct mbdx_property_t {
    char* name;
    char* value;
} mbdx_property_t;

typedef struct mbdx_record_t {
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
} mbdx_record_t;

mbdx_record_t* mbdx_record_create();
mbdx_record_t* mbdx_record_parse(unsigned char* data, unsigned int size);
void mbdx_record_free(mbdx_record_t* mbdx_record);
void mbdx_record_debug(mbdx_record_t* record);

#endif

