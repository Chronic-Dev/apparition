/*
 * device.c
 *
 *  Created on: May 5, 2011
 *      Author: posixninja
 */

/* This is the application Controller.
 *  This file should contain functions to control the device
 *  through lockdownd services. This file acts as a proxy to
 *  the afc, mb2, and nos services and should handle messages.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libimobiledevice/libimobiledevice.h>

#include "device.h"
#include "lockdown.h"

device_t* device_create(const char* uuid) {
	idevice_error_t err = 0;
	device_t* device = NULL;
	device = (device_t*) malloc(sizeof(device_t));
	if (device == NULL) {
		return NULL;
	}
	memset(device, '\0', sizeof(device_t));

	if (uuid == NULL) {
		err = idevice_new(&(device->client), NULL);
		if (err != IDEVICE_E_SUCCESS) {
			fprintf(stderr,
					"No device found with uuid %s, is it plugged in?\n", uuid);
			return NULL;
		}
		idevice_get_uuid(device->client, &device->uuid);

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

void device_free(device_t* device) {
	if (device) {
		if(device->uuid) {
			free(device->uuid);
			device->uuid = NULL;
		}
		free(device);
	}
}

void device_enable_debug() {
	idevice_set_debug_level(3);
}


int device_connect(device_t* device, unsigned short port, idevice_connection_t* connection) {
	idevice_connection_t conn = NULL;
	idevice_error_t device_error = IDEVICE_E_SUCCESS;

	if(device->client == NULL) {
		return -1;
	}

	idevice_error_t device_error = idevice_connect(device->client, port, &conn);
	if(device_error != IDEVICE_E_SUCCESS) {
		return -1;
	}
	*connection = conn;
	return 0;
}
