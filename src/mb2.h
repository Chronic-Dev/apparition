/*
 * mb2.h
 * Functions for lockdownd's mobilebackup2 service
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

#ifndef MB2_H_
#define MB2_H_

#include "device.h"
#include "backup.h"

typedef struct mb2_t {

} mb2_t;

mb2_t* mb2_open(device_t* device);
int mb2_restore(mb2_t* mb2, backup_t* backup);
int mb2_close(mb2_t* mb2);
void mb2_free(mb2_t* mb2);

#endif /* MB2_H_ */
