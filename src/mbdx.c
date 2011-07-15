/**
  * GreenPois0n Apparition - mbdx.c
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
	if(header->magic == MBDX_MAGIC) {
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
	mbdx_header_debug(mbdx->header);
	offset += sizeof(mbdx_header_t);

	count = flip32(mbdx->header->count);
	if(count > 0) {
		// Allocate our struct array
		mbdx->mbdx_records = (mbdx_record_t**) malloc(sizeof(mbdx_record_t*) * count);
		for(i = 0; i < count; i++) {
			record = mbdx_record_parse(mbdx, &data[offset], sizeof(mbdx_record_t));
			if(record == NULL) {
				fprintf(stderr, "Unable to parse mbdx record\n");
				return NULL;
			}
			mbdx->mbdx_records[i] = record;
			//mbdx_record_debug(mbdx->mbdx_records[i]);
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

void mbdx_header_debug(mbdx_header_t* header) {
	fprintf(stderr, "mbdx header:");
	fprintf(stderr, "\tmagic = %x\n", flip32(header->magic));
	fprintf(stderr, "\tversion = %x\n", flip16(header->version));
	fprintf(stderr, "\tcount = %x\n", flip32(header->count));
	fprintf(stderr, "\n");
}
