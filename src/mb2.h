/**
  * GreenPois0n Apparition - mb2.h
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

#ifndef MB2_H_
#define MB2_H_

#include <libimobiledevice/mobilebackup2.h>

#include "device.h"
#include "backup.h"

struct lockdown_t;
typedef struct mb2_t {
	device_t* device;
	struct lockdown_t* lockdown;
	mobilebackup2_client_t client;
	unsigned char *poison;
	size_t poison_length;
	int poison_spilled;
} mb2_t;

mb2_t* mb2_create();
void mb2_free(mb2_t* mb2);

mb2_t* mb2_open(device_t* device);
int mb2_close(mb2_t* mb2);

int mb2_restore(mb2_t* mb2, backup_t* backup);
int mb2_backup(mb2_t* mb2, backup_t** backup);

int mb2_process_messages(mb2_t* mb2, backup_t* backup);
int dlmsg_status_from_string(char *dlmsg);

#endif /* MB2_H_ */
