/*
 * mbdb_record.c
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbdb.h"
#include "byteorder.h"
#include "mbdb_record.h"

mbdb_record_t* mbdb_record_create() {
	mbdb_record_t* record = (mbdb_record_t*) malloc(sizeof(mbdb_record_t));
	if(record == NULL) {
		return NULL;
	}
	memset(record, '\0', sizeof(mbdb_record_t));

	return record;
}


mbdb_record_t* mbdb_record_parse(unsigned char* data) {
	unsigned int offset = 0;
	mbdb_record_t* record  = mbdb_record_create();
	if(record == NULL) {
		fprintf(stderr, "Unable to parse mbdb record\n");
		return NULL;
	}

	//memcpy(record, data, sizeof(mbdb_record_t));

	// Parse Domain
	unsigned char strsize = *((unsigned char*)&data[offset]);
	fprintf(stderr, "%d\n", strsize);
	record->domain = (char*) malloc(strsize);
	if(record->domain == NULL) {
		fprintf(stderr, "Allocation Error!\n");
		return NULL;
	}
	offset++;
	memcpy(record->domain, &data[offset], strsize);
	offset += strlen(record->domain) + 1;

	// Parse Path
	strsize = *((unsigned char*)&data[offset]);
	fprintf(stderr, "%d\n", strsize);
	record->path = (char*) malloc(strsize);
	if(record->path == NULL) {
		fprintf(stderr, "Allocation Error!\n");
		return NULL;
	}
	offset++;;
	memcpy(record->path, &data[offset], strsize);
	offset += strlen(record->path);
	offset += 6;


	record->mode = flip16(*((unsigned short*)&data[offset]));
	offset += 2;

	record->length = flip64(*((unsigned long long*)&data[offset]));
	offset += 8;

	/*
	strsize = *((unsigned char*)&data[offset]);
	fprintf(stderr, "%d\n", strsize);
	record->target = (char*) malloc(strsize);
	if(record->target == NULL) {
		fprintf(stderr, "Allocation Error!\n");
		return NULL;
	}
	offset++;
	memcpy(record->target, &data[offset], strsize);
	offset += strlen(record->target) + 1;
	offset += 6;
	*/

	return record;
}

/*
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
 */

void mbdb_record_free(mbdb_record_t* record) {
	if(record) {
		free(record);
	}
}

void mbdb_record_debug(mbdb_record_t* record) {
	fprintf(stderr, "mbdb record\n");
	//fprintf(stderr, "\tdatahash = %x\n", record->datahash);
	fprintf(stderr, "\tdomain = %s\n", record->domain);
	fprintf(stderr, "\tflag = %x\n", record->flag);
	fprintf(stderr, "\tgid = %x\n", record->gid);
	fprintf(stderr, "\tlength = %llu\n", record->length);
	fprintf(stderr, "\tmode = %x\n", record->mode);
	fprintf(stderr, "\tpath = %s\n", record->path);
	fprintf(stderr, "\tproperties = %x\n", record->properties);
	fprintf(stderr, "\ttarget = %s\t\n", record->target);
	fprintf(stderr, "\ttime1 = %x\n", record->time1);
	fprintf(stderr, "\ttime2 = %x\n", record->time2);
	fprintf(stderr, "\ttime3 = %x\n", record->time3);
	fprintf(stderr, "\tuid = %x\n", record->uid);
	fprintf(stderr, "\tunknown1 = %x\n", record->unknown1);
	fprintf(stderr, "\tunknown2 = 0x%x\n", record->unknown2);
	fprintf(stderr, "\tunknown3 = 0x%x\n", record->unknown3);
}
