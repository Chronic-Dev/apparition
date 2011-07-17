/**
  * GreenPois0n Apparition - device.h
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

#ifndef DEVICE_H_
#define DEVICE_H_

#include <libimobiledevice/libimobiledevice.h>

#include "lockdown.h"

typedef struct device_t {
	idevice_t client;
	lockdown_t* lockdown;

	// Device Info (stick in another struct?)
	unsigned char* uuid;
	unsigned char* ecid;
} device_t;

device_t* device_create();
void device_free(device_t* device);

device_t* device_open(const char* uuid);
void device_close(device_t* device);

void device_enable_debug(unsigned int level);

#endif /* DEVICE_H_ */
