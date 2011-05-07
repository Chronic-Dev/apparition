/*
 * mbdx_record.c
 *
 *  Created on: May 6, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "byteorder.h"
#include "mbdx_record.h"

mbdx_record_t* mbdx_record_create() {
	mbdx_record_t* mbdx_record = (mbdx_record_t*) malloc(sizeof(mbdx_record_t));
	if(mbdx_record == NULL) {
		return NULL;
	}
	memset(mbdx_record, '\0', sizeof(mbdx_record_t));
	return mbdx_record;
}


mbdx_record_t* mbdx_record_parse(unsigned char* data, unsigned int size) {
	mbdx_record_t* mbdx_record  = mbdx_record_create();
	if(mbdx_record == NULL) {
		fprintf(stderr, "Unable to parse mbdx record\n");
		return NULL;
	}

	if(size < sizeof(mbdx_record)) {
		fprintf(stderr, "Invalid record\n");
		return NULL;
	}
	memcpy(mbdx_record, data, sizeof(mbdx_record));
	return mbdx_record;
}

void mbdx_record_free(mbdx_record_t* mbdx_record) {
	if(mbdx_record) {
		free(mbdx_record);
	}
}

void mbdx_record_debug(mbdx_record_t* record) {
	fprintf(stderr, "mbdx record:\n");
	fprintf(stderr, "\tkey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", record->key[0],
			record->key[1], record->key[2], record->key[3], record->key[4], record->key[5], record->key[6], record->key[7],
			record->key[8], record->key[9], record->key[10], record->key[11], record->key[12], record->key[13], record->key[14],
			record->key[15], record->key[16], record->key[17], record->key[18], record->key[19]);
	fprintf(stderr, "\toffset: %u\n",  flip32(record->offset));
	fprintf(stderr, "\tmode: %d\n", record->mode);
	fprintf(stderr, "\n");
}
