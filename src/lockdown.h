/**
  * GreenPois0n Apparition - lockdown.h
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

#ifndef LOCKDOWN_H_
#define LOCKDOWN_H_

#include <libimobiledevice/lockdown.h>

#include "afc.h"
#include "mb2.h"
#include "nos.h"
#include "crashreporter.h"

struct device_t;
typedef struct lockdown_t {
	// lockdownd client if we're connected,
	//  and our device so we're currently connected to
	struct device_t *device;
	lockdownd_client_t client;

	// Easy and quick access to our lockdown services
	afc_t* afc;
	mb2_t* mb2;
	nos_t* nos;
	crashreporter_t* crashreporter;
} lockdown_t;

lockdown_t* lockdown_open(struct device_t* device);
int lockdown_start_service(lockdown_t* lockdown, const char* service, uint16_t* port);
int lockdown_stop_service(lockdown_t* lockdown, const char* service);
int lockdown_close(lockdown_t* lockdown);
void lockdown_free(lockdown_t* lockdown);

#endif /* LOCKDOWN_H_ */
