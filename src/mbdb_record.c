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
	/*
	fprintf(stderr, "mbdx record:\n");
	fprintf(stderr, "\tkey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", record->key[0],
			record->key[1], record->key[2], record->key[3], record->key[4], record->key[5], record->key[6], record->key[7],
			record->key[8], record->key[9], record->key[10], record->key[11], record->key[12], record->key[13], record->key[14],
			record->key[15], record->key[16], record->key[17], record->key[18], record->key[19]);
	fprintf(stderr, "\toffset: %u\n",  flip32(record->offset));
	fprintf(stderr, "\tmode: %d\n", record->mode);
	fprintf(stderr, "\n");
	*/
}
