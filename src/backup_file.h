/*
 * backup_file.h
 *
 *  Created on: May 7, 2011
 *      Author: posixninja
 */

#ifndef BACKUP_FILE_H_
#define BACKUP_FILE_H_

struct mbdx_record_t;
struct mbdb_record_t;

typedef struct backup_file_t {
	struct mbdx_record* mbdx_record;
	struct mbdb_record* mbdb_record;
} backup_file_t;

backup_file_t* backup_file_create(struct mbdx_record_t* mbdx_record, struct mbdb_record_t* mbdb_record);

#endif /* BACKUP_FILE_H_ */
