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
#include "afc.h"

typedef struct mb2_t {
	lockdown_t* lockdown;
	afc_t* afclient;
	mobilebackup2_client_t client;
} mb2_t;

mb2_t* mb2_open(lockdown_t* lockdown, afc_t* afcs);
int mb2_restore(mb2_t* mb2, backup_t* backup);
int mb2_backup(mb2_t* mb2, backup_t** backup);
int mb2_close(mb2_t* mb2);
void mb2_free(mb2_t* mb2);

#endif /* MB2_H_ */
