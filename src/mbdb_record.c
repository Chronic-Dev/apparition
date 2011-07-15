/**
  * GreenPois0n Apparition - mbdb_record.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbdb.h"
#include "byteorder.h"
#include "mbdb_record.h"

mbdb_record_t* mbdb_record_create() {
	mbdb_record_t* record = (mbdb_record_t*) malloc(sizeof(mbdb_record_t));
	if (record == NULL) {
		return NULL;
	}
	memset(record, '\0', sizeof(mbdb_record_t));

	return record;
}

mbdb_record_t* mbdb_record_parse(unsigned char* data) {
	unsigned int offset = 0;
	mbdb_record_t* record = mbdb_record_create();
	if (record == NULL) {
		fprintf(stderr, "Unable to parse mbdb record\n");
		return NULL;
	}

	//memcpy(record, data, sizeof(mbdb_record_t));

	// Parse Domain
	unsigned char strsize = *((unsigned char*) &data[offset]);
	fprintf(stderr, "%d\n", strsize);
	if (strsize > 0) {
		record->domain = (char*) malloc(strsize);
		if (record->domain == NULL) {
			fprintf(stderr, "Allocation Error!\n");
			return NULL;
		}
		offset++;
		memcpy(record->domain, &data[offset], strsize);
		offset += strlen(record->domain);

	} else {
		record->domain = NULL;
		offset++;
	}

	// Parse Path
	strsize = *((unsigned char*) &data[offset]);
	fprintf(stderr, "%d\n", strsize);
	if (strsize > 0) {
		record->path = (char*) malloc(strsize);
		if (record->path == NULL) {
			fprintf(stderr, "Allocation Error!\n");
			return NULL;
		}
		offset++;
		;
		memcpy(record->path, &data[offset], strsize);
		offset += strlen(record->path);

	} else {
		record->path = NULL;
		offset++;
	}

	// Parse Target
	strsize = *((unsigned char*) &data[offset]);
	fprintf(stderr, "%d\n", strsize);
	if (strsize > 0) {
		record->target = (char*) malloc(strsize);
		if (record->target == NULL) {
			fprintf(stderr, "Allocation Error!\n");
			return NULL;
		}
		offset++;
		;
		memcpy(record->target, &data[offset], strsize);
		offset += strlen(record->target);

	} else {
		record->target = NULL;
		offset++;
	}

	record->datahash = (char*) malloc(6);
	memcpy(record->datahash, &data[offset], 6);
	offset += 6;

	record->mode = flip16(*((unsigned short*) &data[offset]));
	offset += 2;

	record->unknown2 = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->unknown3 = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->uid = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->gid = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->time1 = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->time2 = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->time3 = flip32(*((unsigned int*) &data[offset]));
	offset += 4;

	record->length = flip64(*((unsigned long long*) &data[offset]));
	offset += 8;

	record->flag = *((unsigned char*) &data[offset]);
	offset += 1;

	record->properties = *((unsigned char*) &data[offset]);
	offset += 1;

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
	if (record) {
		free(record);
	}
}

void mbdb_record_debug(mbdb_record_t* record) {
	fprintf(stderr, "mbdb record\n");
	fprintf(stderr, "\tdatahash = 0x%x\n", *record->datahash);
	fprintf(stderr, "\tdomain = %s\n", record->domain);
	fprintf(stderr, "\tflag = %x\n", record->flag);
	fprintf(stderr, "\tgid = %d\n", record->gid);
	fprintf(stderr, "\tlength = %llu\n", record->length);
	fprintf(stderr, "\tmode = %x\n", record->mode);
	fprintf(stderr, "\tpath = %s\n", record->path);
	fprintf(stderr, "\tproperties = %x\n", record->properties);
	fprintf(stderr, "\ttarget = %s\n", record->target);
	fprintf(stderr, "\ttime1 = %d\n", record->time1);
	fprintf(stderr, "\ttime2 = %d\n", record->time2);
	fprintf(stderr, "\ttime3 = %d\n", record->time3);
	fprintf(stderr, "\tuid = %d\n", record->uid);
	//fprintf(stderr, "\tunknown1 = %x\n", record->unknown1);
	fprintf(stderr, "\tunknown2 = 0x%x\n", record->unknown2);
	fprintf(stderr, "\tunknown3 = 0x%x\n", record->unknown3);
}

int mbdb_record_build(mbdb_record_t* record, unsigned char** data, unsigned int* size) {
	unsigned int offset = 0;
	unsigned char* buf = NULL;
	unsigned char strsize = '\0';
	unsigned char data_buf[0x2000];

	// Parse Domain
	if (record->domain != NULL) {
		strsize = strlen(record->domain);
		data_buf[offset] = strsize;
		offset++;

		memcpy(&data_buf[offset], record->domain, strsize);
		offset += strsize;

	} else {
		offset++;
	}

	// Parse Path
	if (record->path != NULL) {
		strsize = strlen(record->path);
		data_buf[offset] = strsize;
		offset++;

		memcpy(&data_buf[offset], record->path, strsize);
		offset += strsize;

	} else {
		offset++;
	}

	// Parse Target
	if (record->target != NULL) {
		strsize = strlen(record->target);
		data_buf[offset] = strsize;
		offset++;

		memcpy(&data_buf[offset], record->target, strsize);
		offset += strsize;

	} else {
		offset++;
	}

	if (record->datahash != NULL) {
		memcpy(&data_buf[offset], record->datahash, 6);
		offset += 6;
	}

	int mode = flip16(record->mode);
	memcpy(&data_buf[offset], &mode, 2);
	offset += 2;

	int unknown2 = flip32(record->unknown2);
	memcpy(&data_buf[offset], &unknown2, 4);
	offset += 4;

	int unknown3 = flip32(record->unknown3);
	memcpy(&data_buf[offset], &unknown3, 4);
	offset += 4;

	int uid = flip32(record->uid);
	memcpy(&data_buf[offset], &uid, 4);
	offset += 4;

	int gid = flip32(record->gid);
	memcpy(&data_buf[offset], &gid, 4);
	offset += 4;

	int time1 = flip32(record->time1);
	memcpy(&data_buf[offset], &time1, 4);
	offset += 4;

	int time2 = flip32(record->time2);
	memcpy(&data_buf[offset], &time2, 4);
	offset += 4;

	int time3 = flip32(record->time3);
	memcpy(&data_buf[offset], &time3, 4);
	offset += 4;

	unsigned long long length = flip64(record->length);
	memcpy(&data_buf[offset], &length, 8);
	offset += 8;

	int flag = record->flag;
	memcpy(&data_buf[offset], &flag, 1);
	offset++;

	int prop = record->properties;
	memcpy(&data_buf[offset], &prop, 1);
	offset++;

	*data = data_buf;
	*size = offset;
	return 0;
}
