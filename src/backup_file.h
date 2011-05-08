/*
 * backup_file.h
 *
 *  Created on: May 7, 2011
 *      Author: posixninja
 */

#ifndef BACKUP_FILE_H_
#define BACKUP_FILE_H_

#include "mbdx_record.h"
#include "mbdb_record.h"

typedef struct backup_file_t {
	mbdx_record_t* mbdx_record;
	mbdb_record_t* mbdb_record;
} backup_file_t;

backup_file_t* backup_file_create();

#endif /* BACKUP_FILE_H_ */
