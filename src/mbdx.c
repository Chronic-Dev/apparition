/*
 * mbdx.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbdx.h"

static unsigned int flip32(unsigned int value) {
	unsigned int ret = 0;
	ret |= (value & 0xFF000000) >> (8*2);
	ret |= (value & 0x00FF0000) >> (8*1);
	ret |= (value & 0x0000FF00) << (8*1);
	ret |= (value & 0x000000FF) << (8*2);
	return ret;
}

mbdx_t* mbdx_create() {
	int err = 0;
	unsigned int db_size = 0;
	unsigned char* db_data = NULL;

	mbdx_t* mbdx = NULL;
	mbdx_header_t* header = NULL;
	mbdx_record_t* record = NULL;
	mbdx_record_t** records = NULL;

	mbdx = (mbdx_t*) malloc(sizeof(mbdx_t));
	if(mbdx == NULL) {
		return NULL;
	}
	memset(mbdx, '\0', sizeof(mbdx_t));

	err = file_read("Manifest.mbdx", &db_data, &db_size);
	if(err < 0) {
		fprintf(stderr, "Unable to read mbdx file\n");
		free(mbdx);
		return NULL;
	}

	header = (mbdx_header_t*) db_data;
	if(memcmp(header->magic, MBDX_MAGIC, 6) != 0) {
		free(db_data);
		free(mbdx);
		return NULL;
	}

	mbdx->header = malloc(sizeof(mbdx_header_t));
	if(mbdx->header == NULL) {
		free(db_data);
		free(mbdx);
		return NULL;
	}
	memset(mbdx->header, '\0', sizeof(mbdx_header_t));
	memcpy(mbdx->header, db_data, sizeof(mbdx_header_t));

	if(header->count != 0) {
		mbdx->records = (mbdx_record_t**) malloc(sizeof(mbdx_record_t*) * header->count);
	}

	int i = 0;
	for(i = 0; i < header->count; i++) {
		record =  (mbdx_record_t*) &db_data[sizeof(mbdx_record_t) * i];
		if(record->offset != 0) {
			mbdx->records[i] = (mbdx_record_t*) malloc(sizeof(mbdx_record_t));
			if(mbdx->records[i] == NULL) {
				break;
			}
			memset(mbdx->records[i], '\0', sizeof(mbdx_record_t));
			memcpy(mbdx->records[i], &db_data[sizeof(mbdx_record_t) * i], sizeof(mbdx_record_t));
			mbdx_debug_header(mbdx->header);
			mbdx_debug_record(mbdx->records[i]);
		}
	}
}

void mbdx_free(mbdx_t* mbdx) {
	if(mbdx) {
		free(mbdx);
	}
}

/*
typedef struct mbdx_header_t {
    unsigned char magic[6];		  // "mbdx\2\0"
    unsigned int count;		  // count
} mbdx_header_t;
 */
void mbdx_debug_header(mbdx_header_t* header) {
	fprintf(stderr, "mbdx header:\n");
	fprintf(stderr, "\tmagic: %02x %02x %02x %02x %02x %02x\n", header->magic[0], header->magic[1], header->magic[2], header->magic[3], header->magic[4], header->magic[5]);
	fprintf(stderr, "\tcount: %u\n",  flip32(header->count));
	fprintf(stderr, "\n");
}

/*
typedef struct mbdx_record_t {
    unsigned char key[20];		  // key / filename
    unsigned int offset;		  // offset of the mbdb file (+ 7 bytes)
    unsigned short mode;		  // filemode: Axxx = symlink, 4xxx = dir, 8xxx = file
} mbdx_record_t;
 */
void mbdx_debug_record(mbdx_record_t* record) {
	fprintf(stderr, "mbdx record:\n");
	fprintf(stderr, "\tkey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", record->key[0],
			record->key[1], record->key[2], record->key[3], record->key[4], record->key[5], record->key[6], record->key[7],
			record->key[8], record->key[9], record->key[10], record->key[11], record->key[12], record->key[13], record->key[14],
			record->key[15], record->key[16], record->key[17], record->key[18], record->key[19]);
	fprintf(stderr, "\toffset: %u\n",  flip32(record->offset));
	fprintf(stderr, "\tmode: %d\n", record->mode);
	fprintf(stderr, "\n");
}
