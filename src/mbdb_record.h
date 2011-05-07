/*
 * mbdb_record.h
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#ifndef MBDB_RECORD_H_
#define MBDB_RECORD_H_

struct mbdb_record_t {
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
} __attribute__((__packed__));

typedef struct mbdb_record_t mbdb_record_t;

mbdb_record_t* mbdb_record_create();
mbdb_record_t* mbdb_record_parse(unsigned char* data, unsigned int size);
void mbdb_record_debug(mbdb_record_t* record);
//void mbdb_record_free(mbdb_record_t* record);

#endif /* MBDB_RECORD_H_ */
