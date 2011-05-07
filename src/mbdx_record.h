#ifndef mbdx_record_H
#define mbdx_record_H

typedef struct mbdx_property_t {
    char* name;
    char* value;
} mbdx_property_t;

typedef struct mbdx_record_t {
    unsigned char key[20];		  // key / filename
    unsigned int offset;		  // offset of the mbdb file (+ 7 bytes)
    unsigned short mode;		  // filemode: Axxx = symlink, 4xxx = dir, 8xxx = file
} __attribute__((__packed__));

mbdx_record_t* mbdx_record_create();
mbdx_record_t* mbdx_record_parse(unsigned char* data, unsigned int size);
void mbdx_record_free(mbdx_record_t* mbdx_record);
void mbdx_record_debug(mbdx_record_t* record);

#endif

