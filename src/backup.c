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
#include <dirent.h>

#include "mbdb.h"
#include "mbdx.h"
#include "backup.h"
#include "byteorder.h"

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

backup_t* backup_open(const char* directory, const char* uuid) {
	int i = 0;
	int err = 0;
	unsigned int count = 0;
	char mbdb_manifest[512];
	char mbdx_manifest[512];
	backup_t* backup = backup_create();
	if(backup == NULL) {
		fprintf(stderr, "Unable to create backup object\n");
		return NULL;
	}

	memset(mbdb_manifest, '\0', sizeof(mbdb_manifest));
	snprintf(mbdb_manifest, sizeof(mbdb_manifest)-1, "%s/%s/Manifest.mbdb", directory, uuid);
	backup->mbdb = mbdb_open(mbdb_manifest);
	if(backup->mbdb == NULL) {
		fprintf(stderr, "Unable to open mbdb manifest\n");
		return NULL;
	}

	memset(mbdx_manifest, '\0', sizeof(mbdx_manifest));
	snprintf(mbdx_manifest, sizeof(mbdx_manifest)-1, "%s/%s/Manifest.mbdx", directory, uuid);
	backup->mbdx = mbdx_open(mbdx_manifest);
	if(backup->mbdx == NULL) {
		fprintf(stderr, "Unable to open mbdx manifest\n");
		return NULL;
	}

	if(backup->mbdx) {
		count = flip32(backup->mbdx->header->count);
		if(count > 0) {
			// Allocate backup_t backup_file pointer array
			backup->files = (backup_file_t**) malloc(sizeof(backup_file_t*) * count);
			if(backup->files == NULL) {
				fprintf(stderr, "Allocation Error!!\n");
				return NULL;
			}
			memset(backup->files, '\0', sizeof(backup_file_t*) * count);

			for(i = 0; i < count; i++) {
				// Now allocate a backup_file_t object for each item in the array
				backup_file_t* file = (backup_file_t*) malloc(sizeof(backup_file_t));
				if(file == NULL) {
					fprintf(stderr, "Allocation Error!!\n");
					return NULL;
				}
				memset(file, '\0', sizeof(backup_file_t));
				backup->files[i] = file;

				// Link appropriate mbdx record entry and mbdb record
				file->mbdx_record = backup->mbdx->mbdx_records[i];
				mbdx_record_debug(file->mbdx_record);
				file->mbdb_record = mbdb_get_record(backup->mbdb, file->mbdx_record->offset);
				mbdb_record_debug(file->mbdb_record);
				backup->count++;
			}
		}
	}

	return backup;
}

int backup_save(backup_t* backup, const char* directory, const char* uuid) {
	int i = 0;
	unsigned char backup_dir[512];
	unsigned char mbdx_manifest[512];
	unsigned char mbdb_manifest[512];
	if(backup == NULL) {
		return -1;
	}

	memset(backup_dir, '\0', sizeof(backup_dir));
	snprintf(backup_dir, sizeof(backup_dir)-1, "%s/%s", directory, uuid);
	DIR* d = opendir(backup_dir);
	if(d == NULL) {
		fprintf(stderr, "Unable to open backup directory\n");
		return -1;
	}

	memset(mbdx_manifest, '\0', sizeof(mbdx_manifest));
	snprintf(mbdx_manifest, sizeof(mbdx_manifest)-1, "%s/%s/Manifest.mbdx", directory, uuid);
	FILE* mbdx_fd = fopen(mbdx_manifest, "w");
	if(mbdx_fd == NULL) {
		fprintf(stderr, "Unable to open mbdx manifest\n");
		return -1;
	}

	memset(mbdb_manifest, '\0', sizeof(mbdb_manifest));
	snprintf(mbdb_manifest, sizeof(mbdb_manifest)-1, "%s/%s/Manifest.mbdb", directory, uuid);
	FILE* mbdb_fd = fopen(mbdb_manifest, "w");
	if(mbdb_fd == NULL) {
		fprintf(stderr, "Unable to open mbdb manifest\n");
		return -1;
	}

	// Write mbdx header

	if(backup->count > 0) {
		for(i = 0; i < backup->count; i++) {
			backup_file_t* file = backup->files[i];
			// Make sure file exists
			// Write mbdb record and get offset
			// Write mbdx record and mbdb offset
		}
	}

	fclose(mbdx_fd);
	fclose(mbdb_fd);
	closedir(d);

	return 0;
}

int backup_close(backup_t* backup) {
	return -1;
}

int backup_add_file(backup_t* backup, backup_file_t* file){
	// Hash the file and write it out

	// Allocate new mbdx_record
	// Add record to mbdx object

	// Allocate new mbdb_record
	// Add record to mbdb object
	return 0;
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
