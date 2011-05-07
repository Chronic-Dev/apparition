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

#include "lockdown.h"
#include "device.h"

device_t* device_create(const char* uuid) {
	int i = 0;
	idevice_error_t err = 0;
	int ret = 0;
	device_t* device = NULL;
	device = (device_t*) malloc(sizeof(device_t));
		//device->client = (idevice_t) malloc(sizeof(idevice_t));
	if (device == NULL) {
		return NULL;
	}
	memset(device, '\0', sizeof(device_t));
	if (uuid == NULL)
	{
		err = idevice_new(&(device->client), NULL);
	} else
	{
		err = idevice_new(&(device->client), uuid);
	}
	if (err != IDEVICE_E_SUCCESS) {
		fprintf(stderr, "No device found with uuid %s, is it plugged in?\n", uuid);
		return NULL;
	}
	return device;
}

void device_free(device_t* device) {
	if (device) {
		free(device);
	}
}
