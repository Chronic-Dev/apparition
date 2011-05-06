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

mbdx_t* mbdx_create() {
	int err = 0;
	unsigned int db_size = 0;
	unsigned char* db_data = NULL;

	mbdx_t* mbdx = NULL;
	mbdx_header_t* header = NULL;
	mbdx_record_t* record = NULL;

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
	if(strncmp(header->magic, MBDX_MAGIC, 6) != 0) {
		free(db_data);
		free(mbdx);
		return NULL;
	}
}

void mbdx_free(mbdx_t* mbdx) {
	if(mbdx) {
		free(mbdx);
	}
}
