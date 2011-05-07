/*
 * mbdb_record.c
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "byteorder.h"
#include "mbdb_record.h"

mbdb_record_t* mbdb_record_create() {
	mbdb_record_t* mbdb_record = (mbdb_record_t*) malloc(sizeof(mbdb_record_t));
	if(mbdb_record == NULL) {
		return NULL;
	}
	memset(mbdb_record, '\0', sizeof(mbdb_record_t));
	return mbdb_record;
}


mbdb_record_t* mbdb_record_parse(unsigned char* data, unsigned int size) {
	mbdb_record_t* mbdb_record  = mbdb_record_create();
	if(mbdb_record == NULL) {
		fprintf(stderr, "Unable to parse mbdx record\n");
		return NULL;
	}

	if(size < sizeof(mbdb_record)) {
		fprintf(stderr, "Invalid record\n");
		return NULL;
	}
	memcpy(mbdb_record, data, sizeof(mbdb_record));
	return mbdb_record;
}

void mbdb_record_free(mbdb_record_t* mbdb_record) {
	if(mbdb_record) {
		free(mbdb_record);
	}
}

void mbdb_record_debug(mbdb_record_t* record) {
	fprintf(stderr, "mbdb record\n");
	fprintf(stderr, "\tdatahash = %x\n", record->datahash);
	fprintf(stderr, "\tdomain = %x\n", record->domain);
	fprintf(stderr, "\tflag = %x\n", record->flag);
	fprintf(stderr, "\tgid = %x\n", record->gid);
	fprintf(stderr, "\tlength = %x\n", record->length);
	fprintf(stderr, "\tmode = %x\n", record->mode);
	fprintf(stderr, "\tpath = %s\n", record->path);
	fprintf(stderr, "\tproperties = %x\n", record->properties);
	fprintf(stderr, "\ttarget = %s\t\n", record->target);
	fprintf(stderr, "\ttime1 = %x\n", record->time1);
	fprintf(stderr, "\ttime2 = %x\n", record->time2);
	fprintf(stderr, "\ttime3 = %x\n", record->time3);
	fprintf(stderr, "\tuid = %x\n", record->uid);
	fprintf(stderr, "\tunknown1 = 0x%x\n", record->unknown1);
	fprintf(stderr, "\tunknown2 = 0x%x\n", record->unknown2);
	fprintf(stderr, "\tunknown3 = 0x%x\n", record->unknown3);
}
