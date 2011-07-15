/**
  * GreenPois0n Apparition - mbdb.c
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

mbdb_t* apparition_mbdb = NULL;

mbdb_t* mbdb_create() {
	if(apparition_mbdb == NULL) {
		mbdb_t* mbdb = NULL;

		mbdb = (mbdb_t*) malloc(sizeof(mbdb_t));
		if(mbdb == NULL) {
			return NULL;
		}
		memset(mbdb, '\0', sizeof(mbdb_t));
		apparition_mbdb = mbdb;
	}
	return apparition_mbdb;
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

	mbdb->data = (unsigned char*) malloc(size);
	if(mbdb->data == NULL) {
		fprintf(stderr, "Allocation Error!!\n");
		return NULL;
	}
	memset(mbdb->data, '\0', size);
	memcpy(mbdb->data, data, size);
	mbdb->size = size;

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

mbdb_record_t* mbdb_get_record(mbdb_t* mbdb, unsigned int offset) {
	unsigned int off = flip32(offset)+7;
	fprintf(stderr, "offset = %x; off = %x\n", offset, off);
	fprintf(stderr, "%02x %02x %02x %02x\n", mbdb->data[off], mbdb->data[off+1], mbdb->data[off+2], mbdb->data[off+3]);
	fprintf(stderr, "%02x %02x %02x %02x\n", mbdb->data[off+4], mbdb->data[off+5], mbdb->data[off+6], mbdb->data[off+7]);
	fprintf(stderr, "%02x %02x %02x %02x\n", mbdb->data[off+8], mbdb->data[off+9], mbdb->data[off+10], mbdb->data[off+11]);
	mbdb_record_t* record = mbdb_record_parse(&(mbdb->data)[off]);
	return record;
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
