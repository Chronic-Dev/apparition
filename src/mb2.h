/*
 * mb2.h
 * Functions for lockdownd's mobilebackup2 service
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef MB2_H_
#define MB2_H_

#include <libimobiledevice/mobilebackup2.h>

#include "device.h"
#include "backup.h"
#include "lockdown.h"

typedef struct mb2_t {
	device_t* device;
	lockdown_t* lockdown;
	mobilebackup2_client_t client;
	unsigned char *poison;
	size_t poison_length;
	int poison_spilled;
} mb2_t;

mb2_t* mb2_create(lockdown_t* lockdown);
int mb2_restore(mb2_t* mb2, backup_t* backup);
int mb2_process_messages(mb2_t* mb2, backup_t* backup);
int dlmsg_status_from_string(char *dlmsg);
int mb2_backup(mb2_t* mb2, backup_t** backup);
int mb2_close(mb2_t* mb2);
void mb2_free(mb2_t* mb2);

#endif /* MB2_H_ */
