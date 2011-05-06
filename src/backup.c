/*
 * backup.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

/* This is the application Model
 *  Functions in this file should handle openning, reading, and parsing
 *  backup directories. it should allow access to mbdb and mbdx, as well
 *  as extract and injecting files into the databases.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbdb.h"
#include "mbdx.h"
#include "backup.h"

backup_t* backup_create() {
	int err = 0;
	backup_t* backup = NULL;

	backup = (backup_t*) malloc(sizeof(backup_t));
	if(backup == NULL) {
		return NULL;
	}
	memset(backup, '\0', sizeof(backup_t));

	backup->mbdb = mbdb_create();
	backup->mbdx = mbdx_create();

	return backup;
}

backup_t* backup_open(const char* directory) {
	int err = 0;
	backup_t* backup = backup_create();
	if(backup == NULL) {
		fprintf(stderr, "Unable to create backup object\n");
		return NULL;
	}

	return backup;
}

int backup_add_file(backup_t* backup, const char* local, const char* remote){
	//TODO: Implement Me
	return -1;
}

void backup_free(backup_t* backup) {
	if(backup) {
		if(backup->mbdb) {
			mbdb_free(backup->mbdb);
		}
		if(backup->mbdx) {
			mbdx_free(backup->mbdx);
		}
		free(backup);
	}
}
