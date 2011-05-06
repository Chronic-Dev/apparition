/*
 * backup.h
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef BACKUP_H_
#define BACKUP_H_

#include <libimobiledevice/mobilebackup2.h>

#include "lockdown.h"

typedef struct backup_t {
	lockdown_t* lockdown;
	mobilebackup2_client_t client;
} backup_t;

backup_t* backup_create(lockdown_t* lockdown);
int backup_add_file(backup_t* backup, const char* local, const char* remote);
void backup_free(backup_t* backup);

#endif /* BACKUP_H_ */
