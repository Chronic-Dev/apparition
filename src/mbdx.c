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
#include "mbdx_record.h"

mbdx_t* mbdx_create() {
	int err = 0;
	unsigned int db_size = 0;
	unsigned char* db_data = NULL;

	mbdx_t* mbdx = NULL;

	mbdx = (mbdx_t*) malloc(sizeof(mbdx_t));
	if(mbdx == NULL) {
		return NULL;
	}
	memset(mbdx, '\0', sizeof(mbdx_t));

	return mbdx;
}

mbdx_t* mbdx_parse(unsigned char* data, unsigned int size) {
	int i = 0;
	unsigned int count = 0;
	unsigned int offset = 0;

	mbdx_t* mbdx = NULL;
	mbdx_header_t* header = NULL;
	mbdx_record_t* record = NULL;

	mbdx = mbdx_create();
	if(mbdx == NULL) {
		fprintf(stderr, "Unable to create mbdx\n");
		return NULL;
	}

	header = (mbdx_header_t*) data;
	if(strncmp(header->magic, MBDX_MAGIC, 6) != 0) {
		fprintf(stderr, "Unable to identify this filetype\n");
		return NULL;
	}

	// Copy in our header data
	mbdx->header = (mbdx_header_t*) malloc(sizeof(mbdx_header_t));
	if(mbdx->header == NULL) {
		fprintf(stderr, "Allocation error\n");
		return NULL;
	}
	memset(mbdx->header, '\0', sizeof(mbdx_header_t));
	memcpy(mbdx->header, &data[offset], sizeof(mbdx_header_t));
	offset += sizeof(mbdx_header_t);

	count = mbdx->header->count;
	if(count > 0) {
		// Allocate our struct array
		mbdx->records = (mbdx_record_t**) malloc(sizeof(mbdx_record_t*) * count);
		for(i = 0; i < count; i++) {
			record = mbdx_record_parse(&data[offset], sizeof(mbdx_record_t));
			mbdx->records[i] = record;
			mbdx_record_debug(record);
			offset += sizeof(mbdx_record_t);
		}
	}

	return mbdx;
}

mbdx_t* mbdx_open(unsigned char* file) {
	int err = 0;
	unsigned int size = 0;
	unsigned char* data = NULL;

	mbdx_t* mbdx = NULL;

	err = file_read(file, &data, &size);
	if(err < 0) {
		fprintf(stderr, "Unable to read mbdx file\n");
		return NULL;
	}

	mbdx = mbdx_parse(data, size);
	if(mbdx == NULL) {
		fprintf(stderr, "Unable to parse mbdx file\n");
		return NULL;
	}

	free(data);
	return mbdx;
}

void mbdx_free(mbdx_t* mbdx) {
	if(mbdx) {
		if(mbdx->header) {
			free(mbdx->header);
			mbdx->header = NULL;
		}
		free(mbdx);
	}
}
