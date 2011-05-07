/*
 * backup.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef BACKUP_H_
#define BACKUP_H_

#include "mbdb.h"
#include "mbdx.h"

typedef struct backup_t {
	mbdx_t* mbdx;
	mbdb_t* mbdb;
} backup_t;

backup_t* backup_create();
backup_t* backup_open(const char* directory, const char* uuid);
int backup_add_file(backup_t* backup, const char* local, const char* remote);
void backup_free(backup_t* backup);

#endif /* BACKUP_H_ */
