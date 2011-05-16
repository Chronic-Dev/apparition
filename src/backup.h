/*
 * backup.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef BACKUP_H_
#define BACKUP_H_

#include <plist/plist.h>

#include "mbdb.h"
#include "mbdx.h"
#include "backup_file.h"

typedef struct backup_t {
	char* path;
	mbdx_t* mbdx;
	mbdb_t* mbdb;
	plist_t info;
	plist_t status;
	plist_t manifest;
	backup_file_t** files;
	unsigned int count;
	unsigned char* uuid;
	unsigned char* directory;
} backup_t;

backup_t* backup_create();
backup_t* backup_open(const char* directory, const char* uuid);
plist_t backup_load_info(backup_t* backup);
plist_t backup_load_status(backup_t* backup);
plist_t backup_load_manifest(backup_t* backup);
mbdb_t* backup_load_mbdb(backup_t* backup);
mbdx_t* backup_load_mbdx(backup_t* backup);
int backup_save_info(backup_t* backup);
int backup_save_manifest(backup_t* backup);
int backup_save_status(backup_t* backup);
int backup_save_mbdb(backup_t* backup);
int backup_save_mbdx(backup_t* backup);
int backup_save(backup_t* backup, const char* directory, const char* uuid);
int backup_close(backup_t* backup);
int backup_add_file(backup_t* backup, backup_file_t* file);
void backup_free(backup_t* backup);

#endif /* BACKUP_H_ */
