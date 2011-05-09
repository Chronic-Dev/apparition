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
#include <unistd.h>

#include "mbdb.h"
#include "mbdx.h"
#include "backup.h"
#include "byteorder.h"

backup_t* backup_create() {
	int err = 0;
	backup_t* backup = NULL;

	backup = (backup_t*) malloc(sizeof(backup_t));
	if (backup == NULL) {
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
	if (backup == NULL) {
		fprintf(stderr, "Unable to create backup object\n");
		return NULL;
	}

	//TODO: Free these
	backup->uuid = strdup(uuid);
	backup->directory = strdup(directory);

	memset(mbdb_manifest, '\0', sizeof(mbdb_manifest));
	snprintf(mbdb_manifest, sizeof(mbdb_manifest)-1, "%s/%s/Manifest.mbdb", directory, uuid);
	backup->mbdb = mbdb_open(mbdb_manifest);
	if (backup->mbdb == NULL) {
		fprintf(stderr, "Unable to open mbdb manifest\n");
		return NULL;
	}

	memset(mbdx_manifest, '\0', sizeof(mbdx_manifest));
	snprintf(mbdx_manifest, sizeof(mbdx_manifest)-1, "%s/%s/Manifest.mbdx", directory, uuid);
	backup->mbdx = mbdx_open(mbdx_manifest);
	if (backup->mbdx == NULL) {
		fprintf(stderr, "Unable to open mbdx manifest\n");
		return NULL;
	}

	if (backup->mbdx) {
		count = flip32(backup->mbdx->header->count);
		if (count > 0) {
			// Allocate backup_t backup_file pointer array
			backup->files = (backup_file_t**) malloc(
					sizeof(backup_file_t*) * count);
			if (backup->files == NULL) {
				fprintf(stderr, "Allocation Error!!\n");
				return NULL;
			}
			memset(backup->files, '\0', sizeof(backup_file_t*) * count);

			for (i = 0; i < count; i++) {
				// Now allocate a backup_file_t object for each item in the array
				backup_file_t* file = (backup_file_t*) malloc(
						sizeof(backup_file_t));
				if (file == NULL) {
					fprintf(stderr, "Allocation Error!!\n");
					return NULL;
				}
				memset(file, '\0', sizeof(backup_file_t));
				backup->files[i] = file;

				// Link appropriate mbdx record entry and mbdb record
				file->mbdx_record = backup->mbdx->mbdx_records[i];
				mbdx_record_debug(file->mbdx_record);
				file->mbdb_record = mbdb_get_record(backup->mbdb,
						file->mbdx_record->offset);
				mbdb_record_debug(file->mbdb_record);
				backup->count++;
			}
		}
	}

	return backup;
}

int backup_save(backup_t* backup, const char* directory, const char* uuid) {
	DIR* d = NULL;
	FILE* mbdx_fd = NULL;
	FILE* mbdb_fd = NULL;
	backup_file_t* file = NULL;

	unsigned int i = 0;
	unsigned int bytes = 0;
	unsigned int magic = 0;
	unsigned int count = 0;
	unsigned short version = 0;
	unsigned int file_size = 0;
	unsigned char* file_data = NULL;
	unsigned char backup_dir[512];
	unsigned char key_string[512];
	unsigned char file_string[512];
	unsigned char mbdx_manifest[512];
	unsigned char mbdb_manifest[512];

	if (backup == NULL) {
		return -1;
	}

	memset(backup_dir, '\0', sizeof(backup_dir));
	snprintf(backup_dir, sizeof(backup_dir)-1, "%s/%s", directory, uuid);
	d = opendir(backup_dir);
	if (d == NULL) {
		fprintf(stderr, "Unable to open backup directory\n");
		return -1;
	}

	memset(mbdx_manifest, '\0', sizeof(mbdx_manifest));
	snprintf(mbdx_manifest, sizeof(mbdx_manifest)-1, "%s/%s/Manifest.mbdx", directory, uuid);
	mbdx_fd = fopen(mbdx_manifest, "w");
	if (mbdx_fd == NULL) {
		fprintf(stderr, "Unable to open mbdx manifest\n");
		return -1;
	}

	memset(mbdb_manifest, '\0', sizeof(mbdb_manifest));
	snprintf(mbdb_manifest, sizeof(mbdb_manifest)-1, "%s/%s/Manifest.mbdb", directory, uuid);
	mbdb_fd = fopen(mbdb_manifest, "w");
	if (mbdb_fd == NULL) {
		fprintf(stderr, "Unable to open mbdb manifest\n");
		return -1;
	}

	// Write mbdx header
	// Start with MBDX_MAGIC
	magic = flip32(MBDX_MAGIC);
	bytes = fwrite(&magic, 1, sizeof(MBDX_MAGIC), mbdx_fd);
	if (bytes != sizeof(MBDX_MAGIC)) {
		fprintf(stderr, "Unable to write mbdx magic\n");
		return -1;
	}

	// Version Major 5, Minor 0
	version = flip16(0x0200);
	bytes = fwrite(&version, 1, sizeof(version), mbdx_fd);
	if (bytes != sizeof(version)) {
		fprintf(stderr, "Unable to write mbdx version\n");
		return -1;
	}

	// Number of records
	count = backup->mbdx->header->count;
	bytes = fwrite(&count, 1, sizeof(count), mbdx_fd);
	if (bytes != sizeof(count)) {
		fprintf(stderr, "Unable to write mbdx count\n");
		return -1;
	}

	// Write mbdb header
	// Start with MBDB_MAGIC
	bytes = fwrite(MBDB_MAGIC, 1, sizeof(MBDB_MAGIC), mbdb_fd);
	if (bytes != sizeof(MBDB_MAGIC)) {
		fprintf(stderr, "Unable to write mbdb magic\n");
		return -1;
	}

	// Version Major 5, Minor 0
	version = flip16(0x0500);
	bytes = fwrite(&version, 1, sizeof(version), mbdb_fd);
	if (bytes != sizeof(version)) {
		fprintf(stderr, "Unable to write mbdx version\n");
		return -1;
	}

	// Now write out the files
	if (backup->count > 0) {
		for (i = 0; i < backup->count; i++) {
			file = backup->files[i];

			// Make sure file exists
			memset(key_string, '\0', sizeof(key_string));
			snprintf(key_string, sizeof(key_string)-1, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					file->mbdx_record->key[0], file->mbdx_record->key[1], file->mbdx_record->key[2], file->mbdx_record->key[3],
					file->mbdx_record->key[4], file->mbdx_record->key[5], file->mbdx_record->key[6], file->mbdx_record->key[7],
					file->mbdx_record->key[8], file->mbdx_record->key[9], file->mbdx_record->key[10], file->mbdx_record->key[11],
					file->mbdx_record->key[12], file->mbdx_record->key[13], file->mbdx_record->key[14], file->mbdx_record->key[15],
					file->mbdx_record->key[16], file->mbdx_record->key[17], file->mbdx_record->key[18], file->mbdx_record->key[19]);

			memset(file_string, '\0', sizeof(file_string));
			snprintf(file_string, sizeof(file_string)-1, "%s/%s/%s", directory, uuid, key_string);
			if (access(file_string, R_OK) != 0) {
				memset(file_string, '\0', sizeof(file_string));
				snprintf(file_string, sizeof(file_string)-1, "%s/%s/%s", backup->directory, backup->uuid, key_string);
				bytes = file_read(file_string, &file_data, &file_size);
				if (bytes < 0) {
					fprintf(stderr, "Unable to read file!!\n");
					return -1;
				}

				memset(file_string, '\0', sizeof(file_string));
				snprintf(file_string, sizeof(file_string)-1, "%s/%s/%s", directory, uuid, key_string);
				bytes = file_write(file_string, file_data, file_size);
				if (bytes != file_size) {
					fprintf(stderr, "Size mismatch error!\n");
					return -1;
				}
				//free(file_data);
			}

			// Write mbdb record and get offset
			unsigned int mbdb_record_size = 0;
			unsigned char* mbdb_record_data = NULL;
			int err = mbdb_record_build(file->mbdb_record, &mbdb_record_data,
					&mbdb_record_size);
			if (err < 0) {
				fprintf(stderr, "Unable to build mbdb record for file\n");
				return -1;
			}

			long mbdb_offset = ftell(mbdb_fd);
			bytes = fwrite(mbdb_record_data, 1, mbdb_record_size, mbdb_fd);
			if (bytes < 0) {
				fprintf(stderr, "Unable to write mbdb record data\n");
				return -1;
			}

			// Write mbdx record and mbdb offset
			unsigned int mbdx_record_size = 0;
			file->mbdx_record->offset = flip32(mbdb_offset);
			unsigned char* mbdx_record_data = NULL;
			err = mbdx_record_build(file->mbdx_record, &mbdx_record_data,
					&mbdx_record_size);
			if (err < 0) {
				fprintf(stderr, "Unable to build mbdx record for file\n");
				return -1;
			}

			bytes = fwrite(mbdx_record_data, 1, mbdx_record_size, mbdx_fd);
			if (bytes < 0) {
				fprintf(stderr, "Unable to write mbdx record data\n");
				return -1;
			}
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

int backup_add_file(backup_t* backup, backup_file_t* file) {
	// Hash the file and write it out

	// Allocate new mbdx_record
	// Add record to mbdx object

	// Allocate new mbdb_record
	// Add record to mbdb object
	return 0;
}

void backup_free(backup_t* backup) {
	if (backup) {
		if (backup->mbdb) {
			mbdb_free(backup->mbdb);
		}
		if (backup->mbdx) {
			mbdx_free(backup->mbdx);
		}
		free(backup);
	}
}
