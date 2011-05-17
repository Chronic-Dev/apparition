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

#include <glib.h>
#include <gcrypt.h> //for sha1 data hash
#include "mbdb.h"
#include "mbdx.h"
#include "backup.h"
#include "byteorder.h"

#define SEPERATOR '/'

static void compute_datahash(const char *path, unsigned char *hash_out) {
	gcry_md_hd_t hd = NULL;
	gcry_md_open(&hd, GCRY_MD_SHA1, 0);
	if (!hd) {
		printf("ERROR: Could not initialize libgcrypt/SHA1\n");
		return;
	}

	gcry_md_reset(hd);

	FILE *f = fopen(path, "rb");
	if (f) {
		unsigned char buf[16384];
		size_t len;
		while ((len = fread(buf, 1, 16384, f)) > 0) {
			gcry_md_write(hd, buf, len);
		}
		fclose(f);
		unsigned char *newhash = gcry_md_read(hd, GCRY_MD_SHA1);
		memcpy(hash_out, newhash, 20);
	}
	gcry_md_close(hd);
}

static void print_hash(const unsigned char *hash, int len) {
	int i;
	for (i = 0; i < len; i++) {
		fprintf(stderr, "%02x", hash[i]);
	}
	printf("\n");
}

static int backup_file_copy(const char* inputfile, const char* outputfile) { //copy the file from one location to another

	int err = 0;
	char path[512];
	uint32_t size = 0;
	unsigned char* data = NULL;
	err = file_read(inputfile, &data, &size);
	
	if (err < 0){
		fprintf(stderr, "Failed to open input file at %s!\n", inputfile);
		return -1;
	}
	
	
	err = file_write(outputfile, data, size);
	
	if (err < 0){
		fprintf(stderr, "Failed to open output file at %s!\n", outputfile);
		return -1;
	}
	
	return err;
}

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

plist_t backup_load_info(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	plist_t plist = NULL;
	unsigned char* data = NULL;

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Info.plist", backup->directory, backup->uuid);
	err = file_read(path, &data, &size);
	if (err < 0) {
		fprintf(stderr, "Unable to open Info.plist\n");
		return NULL;
	}
	plist_from_xml(data, size, &plist);
	free(data);

	return plist;
}

plist_t backup_load_manifest(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	plist_t plist = NULL;
	unsigned char* data = NULL;

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Manifest.plist", backup->directory, backup->uuid);
	err = file_read(path, &data, &size);
	if (err < 0) {
		fprintf(stderr, "Unable to open Manifest.plist\n");
		return NULL;
	}
	plist_from_bin(data, size, &plist);
	free(data);

	return plist;
}

plist_t backup_load_status(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	plist_t plist = NULL;
	unsigned char* data = NULL;

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Status.plist", backup->directory, backup->uuid);
	err = file_read(path, &data, &size);
	if (err < 0) {
		fprintf(stderr, "Unable to open Status.plist\n");
		return NULL;
	}
	plist_from_bin(data, size, &plist);
	free(data);

	return plist;
}

mbdb_t* backup_load_mbdb(backup_t* backup) {
	char manifest[512];
	memset(manifest, '\0', sizeof(manifest));
	snprintf(manifest, sizeof(manifest)-1, "%s/%s/Manifest.mbdb", backup->directory, backup->uuid);
	backup->mbdb = mbdb_open(manifest);
	if (backup->mbdb == NULL) {
		fprintf(stderr, "Unable to open mbdb manifest\n");
		return NULL;
	}
	return backup->mbdb;
}

mbdx_t* backup_load_mbdx(backup_t* backup) {
	char manifest[512];
	memset(manifest, '\0', sizeof(manifest));
	snprintf(manifest, sizeof(manifest)-1, "%s/%s/Manifest.mbdx", backup->directory, backup->uuid);
	backup->mbdx = mbdx_open(manifest);
	if (backup->mbdx == NULL) {
		fprintf(stderr, "Unable to open mbdx manifest\n");
		return NULL;
	}
	return backup->mbdx;
}

backup_t* backup_open(const char* directory, const char* uuid) {
	int i = 0;
	int err = 0;
	unsigned int count = 0;
	char mbdb_manifest[512];
	char mbdx_manifest[512];
	unsigned int plist_size = 0;
	unsigned char* plist_data = NULL;

	plist_t info_plist = NULL;
	plist_t status_plist = NULL;
	plist_t manifest_plist = NULL;

	backup_t* backup = backup_create();
	if (backup == NULL) {
		fprintf(stderr, "Unable to create backup object\n");
		return NULL;
	}

	//TODO: Free these
	backup->uuid = strdup(uuid);
	backup->directory = strdup(directory);

	backup->path = (char*) malloc(
			strlen(backup->directory) + strlen(backup->uuid) + 1);
	if (backup->path == NULL) {
		return NULL;
	}
	memset(backup->path, '\0', sizeof(strlen(backup->directory) + strlen(backup->uuid) + 1));
	snprintf(backup->path, strlen(backup->directory) + strlen(backup->uuid), "%s%c%s", backup->directory, SEPERATOR, backup->uuid);

	// Load Info.plist, Manifest.plist, and Status.plist
	backup->info = backup_load_info(backup);
	if (backup->info == NULL) {
		printf("Unable to open Info.plist\n");
		return NULL;
	}

	backup->manifest = backup_load_manifest(backup);
	if (backup->manifest == NULL) {
		printf("Unable to open Manifest.plist\n");
		return NULL;
	}

	backup->status = backup_load_status(backup);
	//error msg
	if (backup->manifest == NULL) {
		printf("Unable to open Status.plist\n");
		return NULL;
	}

	// Load Manifest.mbdb and Manifest.mbdx

	backup->mbdb = backup_load_mbdb(backup);
	if (backup->mbdb == NULL) {
		// error msg
		return NULL;
	}

	backup->mbdx = backup_load_mbdx(backup);
	if (backup->mbdx == NULL) {
		// error msg
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

int backup_save_info(backup_t* backup) { //FIXME: this doesn't seem to work properly, always errors out, the rest of the clone appears to be fine up to this point
	int err = 0;
	char path[512];
	uint32_t size = 0;
	char* data = NULL;
	plist_t info = backup->info;

	plist_to_xml(info, &data, &size); 
	if (data == NULL || size <= 0) {
		fprintf(stderr, "Unable to save Info.plist\n");
		return -1;
	}

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Info.plist", backup->directory, backup->uuid);
	err = file_write(path, data, size);
	if (err < 0) {
		fprintf(stderr, "Unable to save Info.plist\n");
		return -1;
	}

	free(data);
	return 0;
}

int backup_save_manifest(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	char* data = NULL;
	plist_t manifest = backup->manifest;

	plist_to_bin(manifest, &data, &size);
	if (data == NULL || size <= 0) {
		fprintf(stderr, "Unable to save Manifest.plist\n");
		return -1;
	}

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Manifest.plist", backup->directory, backup->uuid);
	err = file_write(path, data, size);
	if (err < 0) {
		fprintf(stderr, "Unable to save Manifest.plist\n");
		return -1;
	}

	free(data);
	return 0;
}

int backup_save_status(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	char* data = NULL;
	plist_t status = backup->status;

	plist_to_bin(status, &data, &size);
	if (data == NULL || size <= 0) {
		fprintf(stderr, "Unable to save Status.plist\n");
		return -1;
	}

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Status.plist", backup->directory, backup->uuid);
	err = file_write(path, data, size);
	if (err < 0) {
		fprintf(stderr, "Unable to save Status.plist\n");
		return -1;
	}

	free(data);
	return 0;
}

int backup_save_mbdb(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	unsigned char* data = NULL;
	mbdb_t* mbdb = backup->mbdb;

	if (data == NULL || size <= 0) {
		fprintf(stderr, "Unable to save Manifest.mbdb\n");
		return -1;
	}

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Manifest.mbdb", backup->directory, backup->uuid);
	err = file_write(path, data, size);
	if (err < 0) {
		fprintf(stderr, "Unable to save Manifest.mbdb\n");
		return -1;
	}

	free(data);
	return 0;
}

int backup_save_mbdx(backup_t* backup) {
	int err = 0;
	char path[512];
	uint32_t size = 0;
	unsigned char* data = NULL;
	mbdx_t* mbdx = backup->mbdx;

	if (data == NULL || size <= 0) {
		fprintf(stderr, "Unable to save Manifest.mbdx\n");
		return -1;
	}

	memset(path, '\0', sizeof(path));
	snprintf(path, sizeof(path)-1, "%s/%s/Manifest.mbdx", backup->directory, backup->uuid);
	err = file_write(path, data, size);
	if (err < 0) {
		fprintf(stderr, "Unable to save Manifest.mbdx\n");
		return -1;
	}

	free(data);
	return 0;
}

int backup_save(backup_t* backup, const char* directory, const char* uuid) {
	DIR* d = NULL;
	FILE* mbdx_fd = NULL;
	FILE* mbdb_fd = NULL;
	backup_file_t* file = NULL;

	int err = 0;
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

	if(backup->uuid) {
		free(backup->uuid);
		backup->uuid = strdup(uuid);
	}
	if(backup->directory) {
		free(backup->directory);
		backup->directory = strdup(directory);
	}

	// Load Info.plist, Manifest.plist, and Status.plist
	err = backup_save_info(backup);
	if (err < 0) {
		printf("Unable to save Info.plist\n");
		return -1;
	}

	err = backup_save_manifest(backup);
	if (err < 0) {
		printf("Unable to save Manifest.plist\n");
		return -1;
	}

	err = backup_save_status(backup);
	if (err < 0) {
		printf("Unable to save Status.plist\n");
		return -1;
	}

	return 0;
}

int backup_close(backup_t* backup) {
	return -1;
}

int backup_add_file(backup_t* backup, backup_file_t* file) {

	// Hash the file and write it out
	
	int err = 0;
	char *thefile = file->filepath;
	char outputpath[512];
	memset(outputpath, '\0', sizeof(outputpath));
	
	char data_hash[20];
	fprintf(stderr, "\n%s SHA1: ", thefile); //just printing out the file for debug
	
	compute_datahash(thefile, data_hash); //compute sha1 datahash for end file name + important for mbdx and mbdb record
	print_hash(data_hash, 20); //debug
	
	char fnamehash[41]; //will be full char of filename
	char *p = fnamehash; //pointer to said char
	
	file->mbdx_record = mbdx_record_create(backup->mbdx);
	
	int i;
	for ( i = 0; i < 20; i++, p += 2 ) {
		snprintf (p, 3, "%02x", (unsigned char)data_hash[i] ); //loop through the bytes to make the full hash
		file->mbdx_record->key[i] = data_hash[i];
	}
	
	file->mbdx_record->mode = 60737; //FIXME: not any kind of proper mode, just randomly grabbed it from an output example
	
	
	/* 
	 
	 
	 FIXME: i know this isn't right because it errored out when trying to write the new output files
	 
	 
	 */
	
	int offset = backup->mbdb->size; //FIXME: not sure how else to get the current / next offset.
	
	printf("mbdb size: %i\n", offset);
	
	offset += sizeof(mbdx_header_t);
	
	printf("new offset: %i\n", offset);
	
	file->mbdx_record->offset = flip32(offset);
	
	snprintf(outputpath, sizeof(outputpath)-1, "%s/%s/%s", backup->directory, backup->uuid, fnamehash); //full output path of the new file
	
	fprintf(stderr, "outputpath: %s\n", outputpath); //debug printing of it
	
	err = backup_file_copy(thefile, outputpath); // copy the file to its new location
	
	file->mbdb_record = mbdb_record_create();
	
	mbdb_property_t *property;
	
	property->name = "";
	property->value = "";
	
	/*
	 
	 FIXME: most of these values are probably not right or proper, just wanted to get an example working adding to the database
	 
	 */
	
	
	mbdb_record_t* mbdb_record = file->mbdb_record;
	mbdb_record->target = "Library/Caches/../../../../etc/";
	mbdb_record->domain = "RootDomain";
	mbdb_record->path = "";
	mbdb_record->length = err;
	mbdb_record->mode = 60737; //kinda randomly chose this, not sure how to do this properly.
	mbdb_record->gid = 0;
	mbdb_record->uid = 0;
	mbdb_record->datahash = "";
	mbdb_record->flag = 0;
	mbdb_record->properties = property;
	mbdb_record->time1 = 0;
	mbdb_record->time2 = 0;
	mbdb_record->time3 = 0;
	mbdb_record->unknown1 = "";
	mbdb_record->unknown2 = 0;
	mbdb_record->unknown3 = 0;
	
	
		//FIXME: is this how we would add a new record to mbdx and mbdb?? not sure.
	
	int count = flip32(backup->mbdx->header->count);
	count++; //right?
	
		//;
	backup->files = (backup_file_t**) realloc(backup->files, sizeof(backup_file_t*) * (count+1));
	
	
	
	backup->files[count] = file;
	
	backup->count++;
	
	mbdx_record_debug(file->mbdx_record);
	mbdb_record_debug(mbdb_record);
	

	// Allocate new mbdx_record
	// Add record to mbdx object

	// Allocate new mbdb_record
	// Add record to mbdb object
	return err;
}

void backup_free(backup_t* backup) {
	if (backup) {
		if (backup->mbdb) {
			mbdb_free(backup->mbdb);
			backup->mbdb = NULL;
		}
		if (backup->mbdx) {
			mbdx_free(backup->mbdx);
			backup->mbdx = NULL;
		}
		if(backup->info) {
			plist_free(backup->info);
			backup->info = NULL;
		}
		if(backup->status) {
			plist_free(backup->status);
			backup->status = NULL;
		}
		if(backup->manifest) {
			plist_free(backup->manifest);
			backup->manifest = NULL;
		}
		free(backup);
	}
}
