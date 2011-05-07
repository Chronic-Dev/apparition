/*
 * mbdb.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbdb.h"

mbdb_t* mbdb_create() {
	mbdb_t* mbdb = NULL;

	mbdb = (mbdb_t*) malloc(sizeof(mbdb_t));
	if(mbdb == NULL) {
		return NULL;
	}
	memset(mbdb, '\0', sizeof(mbdb_t));

	return mbdb;
}

mbdb_t* mbdb_parse(unsigned char* data, unsigned int size) {
	int i = 0;
	unsigned int count = 0;
	unsigned int offset = 0;

	mbdb_t* mbdb = NULL;
	mbdb_header_t* header = NULL;
	mbdb_record_t* record = NULL;

	mbdb = mbdb_create();
	if(mbdb == NULL) {
		fprintf(stderr, "Unable to create mbdb\n");
		return NULL;
	}

	header = (mbdb_header_t*) data;
	if(strncmp(header->magic, MBDB_MAGIC, 6) != 0) {
		fprintf(stderr, "Unable to identify this filetype\n");
		return NULL;
	}

	// Copy in our header data
	mbdb->header = (mbdb_header_t*) malloc(sizeof(mbdb_header_t));
	if(mbdb->header == NULL) {
		fprintf(stderr, "Allocation error\n");
		return NULL;
	}
	memset(mbdb->header, '\0', sizeof(mbdb_header_t));
	memcpy(mbdb->header, &data[offset], sizeof(mbdb_header_t));
	offset += sizeof(mbdb_header_t);

	return mbdb;
}

mbdb_t* mbdb_open(unsigned char* file) {
	int err = 0;
	unsigned int size = 0;
	unsigned char* data = NULL;

	mbdb_t* mbdb = NULL;

	err = file_read(file, &data, &size);
	if(err < 0) {
		fprintf(stderr, "Unable to read mbdb file\n");
		return NULL;
	}

	mbdb = mbdb_parse(data, size);
	if(mbdb == NULL) {
		fprintf(stderr, "Unable to parse mbdb file\n");
		return NULL;
	}

	free(data);
	return mbdb;
}

void mbdb_free(mbdb_t* mbdb) {
	if(mbdb) {
		if(mbdb->header) {
			free(mbdb->header);
			mbdb->header = NULL;
		}
		free(mbdb);
	}
}
