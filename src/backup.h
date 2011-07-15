/**
  * GreenPois0n Apparition - backup.h
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
