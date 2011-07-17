/**
  * GreenPois0n Apparition - device.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/libimobiledevice.h>

#include "device.h"
#include "lockdown.h"

device_t* device_create() {
	device_t* device = (device_t*) malloc(sizeof(device_t));
	if (device) {
		memset(device, '\0', sizeof(device_t));
		device->lockdown = lockdown_create();
		if(device->lockdown == NULL) {
			printf("Unable to create lockdown context\n");
			device_free(device);
			return NULL;
		}
	}
	return device;
}

void device_free(device_t* device) {
	if (device) {
		if(device->lockdown) {
			free(device->lockdown);
			device->lockdown = NULL;
		}
		if(device->uuid) {
			free(device->uuid);
			device->uuid = NULL;
		}
		if(device->client) {
			device_close(device);
		}
		free(device);
	}
}

device_t* device_open(const char* uuid) {
	idevice_error_t err = 0;
	device_t* device = device_create();
	if(device == NULL) {
		printf("Unable to create device context\n");
		return NULL;
	}

	if (uuid == NULL) {
		err = idevice_new(&(device->client), uuid);
		if (err != IDEVICE_E_SUCCESS) {
			fprintf(stderr, "No device found, is it plugged in?\n");
			return NULL;
		}
		idevice_get_uuid(device->client, (char**)&device->uuid);

	} else {
		err = idevice_new(&(device->client), uuid);
		if (err != IDEVICE_E_SUCCESS) {
			fprintf(stderr,
					"No device found with uuid %s, is it plugged in?\n", uuid);
			return NULL;
		}
		device->uuid = strdup(uuid);
	}

	return device;
}

void device_close(device_t* device) {
	if(device->client) {
		idevice_free(device->client);
		device->client == NULL;
	}
}

void device_enable_debug(unsigned int level) {
	idevice_set_debug_level(3);
}
