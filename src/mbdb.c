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
	int err = 0;
	unsigned int db_size = 0;
	unsigned char* db_data = NULL;

	mbdb_t* mbdb = NULL;
	mbdb_header_t* header = NULL;
	mbdb_record_t* record = NULL;

	mbdb = (mbdb_t*) malloc(sizeof(mbdb_t));
	if(mbdb == NULL) {
		return NULL;
	}
	memset(mbdb, '\0', sizeof(mbdb_t));

	err = file_read("Manifest.mbdb", &db_data, &db_size);
	if(err < 0) {
		fprintf(stderr, "Unable to read mbdb file\n");
		free(mbdb);
		return NULL;
	}

	header = (mbdb_header_t*) db_data;
	if(strncmp(header->magic, MBDB_MAGIC, 6) != 0) {
		free(db_data);
		free(mbdb);
		return NULL;
	}


	return mbdb;
}

void mbdb_free(mbdb_t* mbdb) {
	if(mbdb) {
		free(mbdb);
	}
}
