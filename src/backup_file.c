/*
 * backup_file.c
 *
 *  Created on: May 7, 2011
 *      Author: posixninja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backup_file.h"
#include "mbdx_record.h"
#include "mbdb_record.h"

backup_file_t* backup_file_create(mbdx_record_t* mbdx_record, mbdb_record_t* mbdb_record) {
	backup_file_t* file = (backup_file_t*) malloc(sizeof(backup_file_t));
	if(file == NULL) {
		fprintf(stderr, "Allocation Error\n");
		return NULL;
	}
	memset(file, '\0', sizeof(backup_file_t));

	file->mbdb_record = mbdb_record;
	file->mbdx_record = mbdx_record;

	return file;
}
