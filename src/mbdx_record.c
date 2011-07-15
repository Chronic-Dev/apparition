/**
  * GreenPois0n Apparition - mbdx_record.c
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

#include "mbdx.h"
#include "byteorder.h"
#include "mbdx_record.h"

mbdx_t* apparition_mbdx = NULL;

mbdx_record_t* mbdx_record_create(mbdx_t* mbdx) {
	mbdx_record_t* record = (mbdx_record_t*) malloc(sizeof(mbdx_record_t));
	if(record == NULL) {
		return NULL;
	}
	memset(record, '\0', sizeof(mbdx_record_t));

	apparition_mbdx = mbdx;
	return record;
}


mbdx_record_t* mbdx_record_parse(mbdx_t* mbdx, unsigned char* data, unsigned int size) {
	mbdx_record_t* record  = mbdx_record_create(mbdx);
	if(record == NULL) {
		fprintf(stderr, "Unable to parse mbdx record\n");
		return NULL;
	}

	if(size < sizeof(mbdx_record_t)) {
		fprintf(stderr, "Invalid record\n");
		return NULL;
	}
	memcpy(record, data, sizeof(mbdx_record_t));
	return record;
}

void mbdx_record_free(mbdx_record_t* record) {
	if(record) {
		free(record);
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

int mbdx_record_build(mbdx_record_t* record, unsigned char** data, unsigned int* size) {
	unsigned char* buf = (unsigned char*) malloc(sizeof(mbdx_record_t));
	if(buf == NULL) {
		return -1;
	}
	memcpy(buf, record, sizeof(mbdx_record_t));

	*data = buf;
	*size = sizeof(mbdx_record_t);
	return 0;
}
