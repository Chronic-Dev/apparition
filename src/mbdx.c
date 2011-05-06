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
#include "byteorder.h"

mbdx_t* mbdx_create() {
	int err = 0;
	unsigned int offset = 0;
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




	return mbdx;
}

mbdx_t* mbdx_parse(unsigned char* data, unsigned int size)  {
	int err = 0;
	unsigned int offset = 0;

	mbdx_t* mbdx = NULL;
	mbdx_header_t* header = NULL;
	mbdx_record_t* record = NULL;

	header = (mbdx_header_t*) &data[offset];
	if(strncmp(header->magic, MBDX_MAGIC, 6) != 0) {
		return NULL;
	}

	mbdx->header = malloc(sizeof(mbdx_header_t));
	if(mbdx->header == NULL) {
		return NULL;
	}
	memset(mbdx->header, '\0', sizeof(mbdx_header_t));
	memcpy(mbdx->header, &data[offset], sizeof(mbdx_header_t));

	offset += sizeof(mbdx_header_t);

	fprintf(stderr, "Found %u records\n", flip32(header->count));
	//mbdx->records = (mbdx_record_t**) malloc(sizeof(mbdx_record_t*) * (flip32(header->count)+1));

/*
	int i = 0;
	for(i = 0; i < header->count; i++) {
		record =  (mbdx_record_t*) &db_data[offset];
		if(record->offset != 0) {

			printf("Number: %d\n", i);
			mbdx->records[i] = (mbdx_record_t*) malloc(sizeof(mbdx_record_t));
			if(mbdx->records[i] == NULL) {
				break;
			}
			memset(mbdx->records[i], '\0', sizeof(mbdx_record_t));
			memcpy(mbdx->records[i], &db_data[offset], sizeof(mbdx_record_t));
			mbdx_debug_record(mbdx->records[i]);
			mbdx_debug_header(mbdx->header);
			offset += sizeof(mbdx_record_t);
		}
	}
*/
}

mbdx_t* mbdx_open(const char* file) {
	int err = 0;
	unsigned int size = 0;
	unsigned char* data = NULL;

	mbdx_t* mbdx = NULL;

	err = file_read(file, &data, &size);
	if(err < 0) {
		fprintf(stderr, "Unable to open mbdx file\n");
		return NULL;
	}

	mbdx = mbdx_parse(data, size);
	if(mbdx == NULL) {
		fprintf(stderr, "Unable to parse mbdx file\n");
	}
	free(data);
	return mbdx;
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
	fprintf(stderr, "\tmagic: 0x%x\n", header->magic);
	fprintf(stderr, "\tcount: %d\n",  flip32(header->count));
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
